#ifndef SERVER
#define SERVER
#endif
#include "gs.h"

#include  <stdio.h>
#include  <sys/types.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <sys/wait.h>
#include  <sys/param.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <signal.h>
#include  <errno.h>
#include "math.h"

#ifndef SYSV
#ifdef SIGTSTP			/* BSD */
#ifndef BSD
#define BSD 1
#endif
#include  <sys/file.h>
#include  <sys/ioctl.h>
#endif 				/* SIGTSTP (BSD) */
#endif 					/* ! SYSV */

#include <varargs.h>

#ifdef SERVER
#ifdef BSD
/*
 * Under BSD. these server routines use the syslog(3) facility.
 * They don't append a newline, for example.
 */
#include <syslog.h>
#else                               /* NOT BSD */
/*
 * There really ought to be a better way to handle server logging
 * under System V.
 */
#define syslog(a, b)    fprintf(stderr, "%s\n", (b))
#define openlog(a,b,c)  fprintf(stderr, "%s\n", (a))
#endif                              /* BSD */

extern  char  *optarg;
extern  int   optind, opterr;
extern  int   errno;
static  FILE  *inconfig();
static  void  endconfig(), cm_mainloop(),usage();
static  char  *nextline(), *skip();
extern  char  *getcwd(),   *getenv();
extern  char  *index(),    *malloc();

int err_init();
int err_quit();
int err_sys();
int err_ret();
int err_dump();

int debug = 0;
static struct stat statbuf;

/*
* Receiver process dies - Handle child exit signal (SIGCLD)
*/
static void sig_child()
{
#ifdef BSD
    int pid;
    union wait status;
    while ((pid = wait3(&status, WNOHANG, (struct rusage *) 0)) > 0);
#endif
}


static void usage()
{

    printf("/n Usage:\n");
    printf("	gscm [-d] [-f <config file>]\n");
}


main(argc, argv, envp)
int argc;
char *argv[], *envp[];
{
    char *cp, buf[50];
    int fd;
    int pid, i;
    struct sigvec sv;
    int errflag = 0;
    char c;
    char text[256];

    strcpy(pcm->config_file, "gstab");
    pcm->debug = 0;
    err_init(argv[0]);

    while ((c = getopt(argc, argv, "df:")) != -1) {
	switch (c) {
	case 'd':
	    pcm->debug = TRUE;
	    break;
	case 'f':	     /* turn on debugging */
	    strcpy(pcm->config_file, optarg);
	    break;
	case '?':	     /* bad argument */
	    errflag = TRUE;
	    break;
	default:	     /* this can't happen! */
	    fprintf(stderr, "%s: BAD RETURN FROM GETOPT (%d)!\n", c);
	    exit(1);
	}
    }
    if (errflag == TRUE) {
	usage();
	exit(1);
    }

	cp = getenv("GSHOME");
	if (cp != (char *)0)
		strcpy(pcm->homedir, cp);
	else 
		if (getcwd(pcm->homedir, sizeof pcm->homedir) == (char *)0)
			err_sys("Cannot get current working directory.");
	

	if (stat(pcm->homedir,&statbuf)) {
		err_sys("Can't locate path: %s\n",pcm->homedir);
		}
	if (pcm->debug)
		printf("Home directory is %s\n",pcm->homedir);

	sprintf(text,"%s/%s",pcm->homedir,pcm->config_file);
	strcpy(pcm->config_file,text);

    if (pcm->debug) {
	printf("Debug mode ON\n");
	printf("Config file is %s\n", pcm->config_file);
    }



/*
 * Ignore terminal signals if defined (BSD) (BSD only)
 */
#ifdef SIGTTIN		     /* Ignore the tty read by background process */
    signal(SIGTTIN, SIG_IGN);
#endif

#ifdef SIGTTOU		     /* Ignore the tty write by background process */
    signal(SIGTTOU, SIG_IGN);
#endif

#ifdef SIGTSTP		     /* Ignore suspend signal. (^Z usually) */
    signal(SIGTSTP, SIG_IGN);
#endif


/*
 * If not in debug mode then run as a Daemon process. This involves:
 * - fork child process & kill parent
 * - Become leader of a new process group
 * - disassociate the controlling terminal from this process.
 */
    if (!pcm->debug) {
	if ((pid = fork()) < 0)
	    err_sys("Cannot fork.");
	else if (pid > 0)
	    exit(0);

#ifdef SIGTSTP		     /* BSD */
	if (setpgrp(0, getpid()) == -1)	/* Become leader of new proc. group */
	    err_sys("Cant change process group");
					/* Close process * terminal */
	if ((fd = open("/dev/tty", O_RDWR)) >= 0) {	
	    ioctl(fd, TIOCNOTTY, (char *) 0);
	    close(fd);
	}
#else			     /* SYSV */
	if (setpgrp() == -1)
	    err_sys("Can't change process group");
	signal(SIGHUP, SIG_IGN);	/* Let child survive kill of parent */
	if ((pid = fork()) < 0)	/* fork child again to disassociate */
	    err_sys("Can't fork second child");
	else if (pid > 0)
	    exit(0);
	{
				/* Close descriptors in child process. */
	    int f;
	    for (f = 0; f < NOFILE; f++)
		(void) close(f);	
	}
#endif

	errno = 0;
	umask(0);
	chdir("/");	     /* cd to a local (real) filesystem. */
    }

/*
 * Handle child process exit notification.
 */
#ifdef SIGTSTP		     /* If BSD then handle SIGCLD */
    signal(SIGCLD, sig_child);
#else			     /* For SYSV, handler not necessary */
    signal(SIGCLD, SIG_IGN);
#endif

	pcm->cm_pid = pid;

    openlog("cm", LOG_PID | LOG_NOWAIT, LOG_DAEMON);

    if (init_cm() != 0) {
	err_sys("Failed to create CM slot.");
	exit(1);
    }

    cm_mainloop();



    /* Wrap it up... Detach shared memory segment. */

    /* Wait for lock, then Free semaphores */
    gs_sem_close(pcm->cmslot->locksem);
    gs_sem_close(pcm->cmslot->cmsem);
    gs_free(pcm->cmslot);
    if (shmctl(pcm->cm_shmid,IPC_RMID,(struct shmid_ds *)0) < 0)
	err_sys("Failed to remove shared memory");
}

static void cm_mainloop()
{
int done=0;

    while (!done) {
	sleep(15);
	done = 1;
	/* gs_sem_wait(pcm->cmsem); */
    }	
}

static int init_cm()
{
    int status;


    char *cp;
	inconfig();	
	cp = getenv("GSCMKEY");
	if ( cp == (char *)0 ) {
		/* No CM slot key Env variable defined. */
		/* Use the config file path instead. */
		pcm->cm_key = (key_t)ftok(pcm->config_file, FTOKID);
		if (pcm->cm_key == (key_t) -1) {
			err_sys("Error creating key with ftok()");
			exit(1);
			}
	} else {
		pcm->cm_key = (key_t) atoi(cp);
	}
	if (pcm->debug)
		printf("Key is %d\n",pcm->cm_key);

	
	pcm->cm_shmid = shmget(pcm->cm_key,sizeof (CMSlot), 0666 | IPC_CREAT);
	if (pcm->cm_shmid < 0)
		err_sys("Cannot alloc shared memory.");
	pcm->cmslot = (CMSlot *)shmat(pcm->cm_shmid, (char *)0,0);
	if (pcm->cmslot == (CMSlot *)-1)
		err_sys("Cannot attach shared memory.");
	if (pcm->debug)
		printf("Slot address is %d\n",(int)pcm->cmslot);

	pcm->cmslot->locksem=gs_sem_create(pcm->cm_key,0);
	if (pcm->cmslot->locksem < 0 ) 
		err_sys("Cannot create lock semaphore");
	
	pcm->cmslot->cmsem=gs_sem_create(pcm->cm_key+1,0);
	if (pcm->cmslot->cmsem < 0 ) 
		err_sys("Cannot create cm semaphore");
	
      return 0;
}

static FILE *inconfig()
{

    if (pcm->config_fp != NULL) {
	fseek(pcm->config_fp, 0L, L_SET);
	return (pcm->config_fp);
    }
    pcm->config_fp = fopen(pcm->config_file, "r");
    return (pcm->config_fp);
}

static void  endconfig()
{

    if (pcm->config_fp == NULL)
	return;
    fclose(pcm->config_fp);
    pcm->config_fp = NULL;
}


static char *skip(cpp)
char **cpp;
{
    register char *cp = *cpp;
    char *start;

again:
    while (*cp == ' ' || *cp == '\t')
	cp++;
    if (*cp == '\0') {
	char c;

	c = getc(pcm->config_fp);
	ungetc(c, pcm->config_fp);
	if (c == ' ' || c == '\t')
	    if (cp = nextline(pcm->config_fp))
		goto again;
	*cpp = (char *) 0;
	return ((char *) 0);
    }
    start = cp;
    while (*cp && *cp != ' ' && *cp != '\t')
	cp++;
    if (*cp != '\0')
	*cp++ = '\0';
    *cpp = cp;
    return (start);
}

static char *nextline(fd)
FILE *fd;
{
    char line[256];
    char *cp;

    if (fgets(line, sizeof(line), fd) == NULL)
	return ((char *) 0);
    cp = index(line, '\n');
    if (cp)
	*cp = '\0';
    return (line);
}

char *strdup(cp)
char *cp;
{
    char *new;

    if (cp == NULL)
	cp = "";
    new = malloc((unsigned) (strlen(cp) + 1));
    if (new == (char *) 0) {
	syslog(LOG_ERR, "Out of memory.");
	exit(-1);
    }
    strcpy(new, cp);
    return (new);
}

char emsgstr[255] = 
    {
    0
    };



/*
 * identify ourself, for syslog() messages
 *
 * LOG_PID is an option that says prepend each message with out PID.
 * LOG_CONS is an option that says write to console if unable to send
 * the message to syslogd.
 * LOG_DAEMON is our facility
 */

int err_init (ident)
char *ident;
    {
    openlog (ident, (LOG_PID | LOG_CONS), LOG_DAEMON);
    }

/*
 * Fatal error.  Print a message and terminate.
 * Don't dump core and don't print the system's errno value.
 *
 *      err_quit(str, arg1, arg2, ...)
 *
 * The string "str" must specify the conversion specification for any args
 */

/*VARARGS1*/
int err_quit (va_alist)
va_dcl
    {
    va_list args;
    char *fmt;
    
    va_start (args);
    fmt = va_arg (args, char *);
    vsprintf (emsgstr, fmt, args);
    va_end (args);
    
    syslog (LOG_ERR, emsgstr);
    
    exit (1);
    }

/*
 * Fatal error related to a system call.  Print a message and terminate.
 * Don't dump core, but do print the system's errno value and its
 * associated message.
 *
 *      err_sys(str, arg1, arg2, ...)
 *
 * The string "str" must specify the conversion specification for any args
 */

/*VARARGS1*/
int err_sys (va_alist)
va_dcl
    {
    va_list args;
    char *fmt;
    
    va_start (args);
    fmt = va_arg (args, char *);
    vsprintf (emsgstr, fmt, args);
    va_end (args);
    
    perror ();
    syslog (LOG_ERR, emsgstr);
    
    exit (1);
    }

/*
 * Recoverable error.  Print a message and return to caller.
 *
 *      err_ret(str, arg1, arg2, ...)
 *
 * The string "str" must specify the conversion specification for any args
 */

/*VARARGS1*/
int err_ret (va_alist)
va_dcl
    {
    va_list args;
    char *fmt;
    
    va_start (args);
    fmt = va_arg (args, char *);
    vsprintf (emsgstr, fmt, args);
    va_end (args);
    
    perror ();
    syslog (LOG_ERR, emsgstr);
    
    return;
    
    }

/*
 * Fatal error.  Print a message, dump core, (for debugging) and terminate.
 *
 *      err_dump(str, arg1, arg2, ...)
 *
 * The string "str" must specify the conversion specification for any args
 */

/*VARARGS1*/
int err_dump (va_alist)
va_dcl
    {
    va_list args;
    char *fmt;
    
    va_start (args);
    fmt = va_arg (args, char *);
    vsprintf (emsgstr, fmt, args);
    va_end (args);
    
    perror ();
    syslog (LOG_ERR, emsgstr);
    
    abort ();
    exit (1);
    
    }
#endif                              /* SERVER */




