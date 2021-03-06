/********************************************************************
Program:	in.fld		- Internet FinderLink Daemon

Description:	This is the FinderLink host server, it is run
		automatically by the inetd process whenever a
		request is sent to the finderlink port defined
		in the /etc/services file.

		The in.fld program implements the protocol that
		that is used to connect a data stream sent by
		an fl_put to an appropriate data receiver.
		
Usage:		This program is not run interactively.  It is
		started by the inetd daemon and handed a socket
		from which it reads the data stream.

		The program uses the following I/O convention:
		stdin	=  stdout of caller
		stdout  =  stdin of receiver process.
		stderr  = (if not a terminal) /dev/flsrv.{PID}

Setup:
	This program requires that you make an entry in the file
	/etc/inetd.conf as follows:

  finderlink  stream	tcp	wait	root	BINDIR/in.fld in.fld

	where BINDIR is replaced by the path to your installed finder
	system.  This is the same path as found in $FINDER_HOME.

	You must also make an entry for the finderlink network port
	in the file /etc/services as follows:

  finderlink  1526/tcp   FinderLink    #Finderlink host listner

	The above changes should be made by the local system
	administrator.
	

Author:		Julian Carlisle
Date:		Sun Feb 24 23:21:33 PST 1991
********************************************************************/

#include <stdio.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#ifndef vms
#include <pwd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#ifdef SUN4
#include <sys/filio.h>
#else
#define	FD_SET(n, s)	(((s)->fds_bits[0]) |= (1 << n))
#define	FD_CLR(n, s)	(((s)->fds_bits[0]) &= ~(1 << n))
#define	FD_ZERO(s)	bzero((char *)(s), sizeof (*(s)))
#define	FD_ISSET(n, s)	(((s)->fds_bits[0]) & (1 << n))
#endif			     /* SUN4 */
#else
#include <types.h>
#include <time.h>
#include <socket.h>
#include <in.h>
#include <stat.h>
#endif


extern  int errno;
char   *index(), *rindex(), *strncat();
/*VARARGS1*/
int     error();

#define MAXFLCLIENTS 5
int     debug;
int     flsocket;
struct sockaddr_in flAddress;

struct proto
{
    char    c1;
    char    c2;
    int     cnt;
};

enum mclass
{
    FLUSER, FLAPP, FLARG
};
typedef struct
{
    int     class;
    char   *str;
} MSGS;

extern struct servent *getservbyname();
extern char *getenv();

#ifdef vms
#define LOG_WARNING 1
#define LOG_INFO 2
#define LOG_ERR 3
#define LOG_PID 9
#define LOG_DAEMON 10
static void openlog();
static void closelog();
static void syslog();
#define fork vfork
#endif
/*ARGSUSED*/
main(argc, argv)
int     argc;
char  **argv;
{
    struct servent *sp;
    char   *host;
    register int n;
    extern char *optarg;
    extern int optind, opterr;
    char    c,*av[4];
    int     argerr = 0;
    struct linger linger;
    int     on = 1, fromlen, len;
    struct sockaddr_in from;
    struct proto pp;
    char    app[64];
    char    text[256];
    MSGS  pm[32];
    int     class,sock;
    int     debug = 1;
    int 	pid;
    openlog("in.fld", LOG_PID, LOG_DAEMON);
#ifndef vms
    if (getenv("DEBUG"))
#else
    if (1)
#endif
    {
	fprintf(stderr,"Creating socket & binding locally");
	sp = getservbyname("finderlink", "tcp");
	if (sp == (struct servent *) NULL)
	{
	    fprintf(stderr, "Can't find finderlink service\n");
	    exit(1);
	}
	flAddress.sin_family = AF_INET;
	flAddress.sin_addr.s_addr = INADDR_ANY;
	flAddress.sin_port = sp->s_port;

	flsocket = socket(AF_INET, SOCK_STREAM, 0);
	if (flsocket < 0)
	{
	    fprintf(stderr, "Can't open finderlink socket\n");
	    perror("socket");
	    exit(1);
	}
	if (bind(flsocket, &flAddress, sizeof flAddress) < 0)
	{
	    fprintf(stderr, "Can't bind to finderlink address\n");
	    perror("bind");
	    (void) close(flsocket);
	    exit(1);
	}
	listen(flsocket,1);
	fromlen = sizeof(from);
	sock=accept(flsocket, &from,&fromlen);
	dup2(sock, 0);
	close(sock);
	dup2(0, 1);
	dup2(0, 2);
    }
    else
    {

    /* Get the socket address of the peer requesting service. */
	fromlen = sizeof(from);
	if (getpeername(0, &from, &fromlen) < 0)
	{
	    fprintf(stderr, "%s: ", argv[0]);
	    perror("getpeername");
	    _exit(1);
	}
    }




    (void) signal(SIGINT, SIG_DFL);
    (void) signal(SIGQUIT, SIG_DFL);
    (void) signal(SIGTERM, SIG_DFL);
#ifdef DEBUG
    {
	int     t = open("/dev/tty", 2);
	if (t >= 0)
	{
	    ioctl(t, TIOCNOTTY, (char *) 0);
	    (void) close(t);
	}
    }
#endif

 /*
  * First 2 bytes from stream are protocol pattern markers, they should be
  * the ascii characters 'f' & 'l' respectively.  if they arrive in reverse
  * order then we are talking to a machine with different byte ordering
  */
    syslog(LOG_WARNING, "Starting...");
    (void) alarm(60);	     /* allow 1 minute to read before giving up. */
    if (read(0, &c, 1) != 1)
	exit(1);
    pp.c1 = c;
    if (read(0, &c, 1) != 1)
	exit(1);
    pp.c2 = c;
    (void) alarm(0);


/*
* Read rest of protocol header:
*	short msgcnt
*	msgcnt strings ending with '\0'
*/
    len = sizeof(pp.cnt);
    if (read(0, &pp.cnt, len) != len)
    {
	syslog(LOG_WARNING, "Incomplete message count read.");
	exit(1);
    }
    for (n = 0; n < pp.cnt; n++)
    {
	len = sizeof(pm[n].class);
	if (read(0, &pm[n].class, len) != len)
	{
	    syslog(LOG_WARNING, "Failed reading msg class %d\n", n);
	    exit(1);
	}
	getstr(text, sizeof(text), "msgclass");
	pm[n].str = (char *) malloc(strlen(text) + 1);
	strcpy(pm[n].str, text);
	sprintf(text, "got message: %s,class: %d\n",
	    pm[n].str, pm[n].class);
	write(0, text, sizeof(text));
    }


	av[0] = pm[0].str;
	av[1] = pm[1].str;
	av[2] = (char *)0;

	sprintf(text,"/j/fl/net/%s",av[0]);
	pid = fork();
	if(pid > 0)
	{
	    syslog(LOG_WARNING,"starting application server: %s %s %s",
		    text,av[0],av[1]);
	    closelog();
	    execv(text,av);
	    syslog(LOG_ERR,"Cannot start server %s: %m",text);
	    _exit(1);	
	}
	else if (pid == 0)
	{
	    syslog(LOG_WARNING,"in.fld exiting...");
	    closelog();
	    exit(0);
	}
	else
	{
		syslog(LOG_WARNING,"Error starting app server");
	}

    closelog();
    close(0);
    exit(0);
}

#ifdef vms
static void 
openlog()
{ }
static void closelog() {}
static void syslog(){}
#endif

getstr(buf, cnt, err)
char   *buf;
int     cnt;
char   *err;
{
    char    c;

    do
    {
	if (read(0, &c, 1) != 1)
	    exit(1);
	*buf++ = c;
	if (--cnt == 0)
	{
	    error("%s too long\n", err);
	    exit(1);
	}
    } while (c != 0);
}
/*VARARGS1*/
error(fmt, a1, a2, a3)
char   *fmt;
int     a1, a2, a3;
{
    char    buf[BUFSIZ];

    buf[0] = 1;
    (void) sprintf(buf + 1, fmt, a1, a2, a3);
    syslog(LOG_WARNING, buf);
}

