#ifndef SERVER
#define SERVER 1
#endif
#define SYSV 1
#include "gs.h"

/****************************************************************************
Module: GS_CM

Description:
	This is the GeoShare Connection Manager program for Unix based
	systems.  This program runs continously in the background waiting
	for messages to arrive in its message slot.

	The connection manager is responsible for initiating connections
	between Data senders and data receivers.  The application program
	requests connection to a data receiver process by sending a special
	message called an Executive Request, to the CM message slot.
	Executive requests may be sent from either partie in a connection
	during any point in the transfer process.  This is because the
	Connection Manager maintains a seperate message area for Executive
	Requests, and therefor does not have to worry about data collisions.

	Application programs connect to the CM by utilizing the GeoShare
	application programmer interface toolkit.  The API simplifies the
	structure of sender & receiver programs by hiding the vagaries of
	data transfer & synchronization.  Users simply call gs_initialize
	and pass in the name of the receiver program to connect to, as well
	as any arguments that program requires.  gs_initialize returns a
	connection handle which can be used for calls to gs_send(), a
	high-level interface for sending GeoScience.h data structures.

	The operational aspects  of the CM is covered in the document
	'GeoShare Software Design'.


	INSTALLATION PRIVILEDGES
	------------------------
	Note:  This process needs to be run under the ROOT operating system
	account so that it can have the priviledge to start processes under
	various other user-ids other than its own.  This allows the user
	wishing to send data to start up the receiver as himself and thus
	avoid access problems.  If this program is not installed as set-uid
	ROOT then it must be started by the root account, ideally during
	system boot directly from the boot file /etc/rc.local.

	If you choose not to install/use the CM program in a priveledged
	account, you must atleast be sure that the programs and files
	associated with GeoShare be stored in a directory that is not
	accessible for writing by anyone other than the system administrator.

	The connection manager does not allow arbitrary starting of
	processes.  The programs that it can start for a calling application
	are all defined in the configuration file.  Each program is given a
	short name, or alias, that is used by applications when specifying a
	receiver.  The full path of the receiver program is contained
	withing the config file.  If this file is not protected from write
	access by non-system users, an obvious security problem arises;
	users can stick their own entries in this file and have their
	programs executed as ROOT.


	GARBAGE COLLECTION & TRAGEDY RECOVERY
	-------------------------------------
	This program performs regularly scheduled cleanup & monitoring tasks
	by setting the SIGALRM signal and catching it.  The reason for this
	is as follows:

		* If either side of a connection suddenly exits without
		  properly shutting down & releasing its resources the CM
		  will notice this and provide for orderly cleanup.  If a
		  process dies without freeing its semaphores, they will
		  stay allocated until the system is re-booted.  A
		  connection manager running as root can intercede and take
		  over the orphaned resources.

		* If a process gets jammed in a blocking operation, this
		  will be noticed by the CM, which can in turn send the
		  blocked process a signal that causes it to abort its read
		  and try again.

		* Changes and or additions to the CM configuration file
		  gstab can be dynamically picked up  by periodically checking
		  the modification timestamp on the configuration file and
		  re-reading it whenver it shows an update.

	
	Application processes connect to the CM initially by locating and
	attaching to the CM message slot by resolving its 'well-known-address'.
	The CM message slot is a shared memory datastructure that is
	addressable by building a key from the full path of the configuration
	file gstab.  The gstab file is usually located in the directory
	$GSHOME/gstab.


	
Status:

	GSERR_MSGTYPE        - Unknown message type.
	GSERR_NOCONNECT      - Connect attempt failed.
	GSERR_EXECTYPE       - Unknown Executive Request.
	GSERR_SVC_UNAVAIL    - Unknown data service.
	GSERR_SVC_BUSY       - Requested data receiver is single
					     - threaded, and already in use.

	GSERR_SVC_MISMATCH   - The data type sent to the data
					     - service is not supported.

	GSERR_SVC_BADCONTEXT - The context args given to the
					     - receiver are bad or unsupported.

	GSERR_CM_NOTINIT     - The connection manager is not
			   			 - available, request is refused.


Author:
	Julian Carlisle 	 Sat Apr 19 01:23:17 PDT 1991
*****************************************************************************/
#include  <sys/types.h>
#include  <string.h>
#include  <sys/stat.h>
#include  <sys/wait.h>
#include  <sys/param.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <signal.h>
#include  <setjmp.h>
#include  <errno.h>
#include  <math.h>	     /* declare atoi for me. */
#include  <stdarg.h>

#ifndef SYSV
#ifdef SIGTSTP		     /* BSD */
#ifndef BSD
#define BSD 1
#endif
#include  <sys/file.h>
#include  <sys/ioctl.h>
#endif			     /* SIGTSTP (BSD) */
#else
#ifdef BSD
#undef BSD
#endif			     /* ! SYSV */
#endif			     /* ! SYSV */

extern int wait3();
extern int putenv();
extern int chdir();

#ifdef SERVER

GSGBL_t CM, *pcm = &CM;

#define GSIGORINTERVAL  10000/* seconds */
#define WATCHDOGNAME "Boris - Watching %s %d"


extern char *optarg;
extern int optind, opterr;
extern int errno;

extern char *getcwd(), *getenv();
#if 0
extern char *index(), *malloc();
#endif
static void usage();
static void gs_igor();
static void gs_hup();
static void gs_cmtermvec();
static void gs_cmintvec();
static int gs_term_cm();
static int gs_cm_mainloop();
static int gs_init_cm();

int     GsState = 0;

enum states {
	InitState = 0,
	WaitcmsemState,
	ProcessMsgState,
	DoneState,
	GarbageCollectState,
	TimeoutState,
	ChildWaitState
	};
struct {
	enum states state;
	char str[64];
	}cmstatetbl[]  = {
		InitState,			"Initializing",
		WaitcmsemState,		"Waiting for messages on CM semaphore...",
		ProcessMsgState,	"Processing Executing Request",
		DoneState,			"Shutting down connection manager",
		GarbageCollectState,	"Cleaning up dead/locked connections",
		TimeoutState,		"Timed out - Cleaning up & exiting",
		ChildWaitState,		"Waiting for child process to exit"
		};


static proc cmplist = {0,0,0,0,(proc *)0};


proc *gs_findproc(pid)
int pid;
{
	pproc pp;

	for (pp=&cmplist; pp->next; pp=pp->next) {
		if (pp->pid == pid)
			return pp;
		}
	return ((proc *)0);
}

void gs_addproc(pid,type,key,shmid)
int pid;
int type;
int shmid;
key_t key;
{
	pproc pp;


	
	for (pp=&cmplist; pp->next ; pp=pp->next)
		;

	pp->next = (pproc)malloc(sizeof(proc));
	pp->next->pid= pid;
	pp->next->type  =type;
	pp->next->key = key;
	pp->next->shmid = shmid;
	pp->next->next = (pproc)0;

}

void gs_delproc(pid)
int pid;
{
pproc pp;

	for (pp=&cmplist; pp->next; pp=pp->next) {
		if (pp->next->pid == pid) {
			pproc tp;
			tp = pp->next->next;
			free(pp->next);
			pp->next = tp;
		}
	}

}


jmp_buf gs_cmreapenv;

 /*
  * catch SIGTERM signals.
  */
static void gs_cmtermvec()
{
	longjmp(gs_cmreapenv, 1);
}

 /*
  * catch SIGINT signals.
  */
static void gs_cmintvec()
{
	longjmp(gs_cmreapenv, 1);
}

 /*
  * Receiver process dies - Handle child exit signal (SIGCLD) This is not
  * needed on SYSV systems like the Sun.
  */
#if defined(BSD) || defined(sun)
static void gs_cm_sigcld()
{
	int     pid;
	int status;
	int oldstate;

	oldstate = GsState;
	GsState = ChildWaitState;
	while ((pid = wait3(&status, WNOHANG, (struct rusage *) 0)) > 0)
		gs_log("cm:Child %d has exited. Status was %lu",pid,status);
	GsState = oldstate;
	signal(SIGCLD,gs_cm_sigcld);
}
#endif




 /*
  * GS_IGOR - Interval Wakeup Task Handler
  * 
  * Perform connection monitoring & ipc resource cleanup chores every 5 seconds
  * using SIGALRM.  Watch for sender & receiver processes that die for some
  * reason while they are still holding locks on slot semaphores.  The CM is
  * responsible for cleaning up and releasing resources that are abandoned by
  * crashed clients.
  * 
  */
static int cm_timeout = FALSE;
static void gs_igor()
{
	static int nalarms = 0;
	GsState = GarbageCollectState;

	/* Check for dead connections... */
	nalarms++;
	if (nalarms > MAXTRYS) {
		cm_timeout = TRUE;
		GsState = TimeoutState;
		nalarms = 0;
	}
}

static void gs_hup()
{
	cm_timeout = TRUE;
}

static void gs_usr1()
{
int oldalrm;
	pproc pp;
oldalrm = alarm(0);
	gs_log("cm:SIGUSR1 - Dumping states to logfile...");
	gs_log("cm:state is: %s",cmstatetbl[GsState].str);
	gs_sem_dump("  (CM)	locksem", pcm->cmslot->locksem);
	gs_sem_dump("  (CM)	msgsem", pcm->cmslot->msgsem);
	gs_sem_dump("  (CM)	cmsem", pcm->cmslot->cmsem);

	gs_log("Processes registered with cm: (type 1=CM, 0=Client)");
	for (pp=&cmplist; pp; pp=pp->next) {
		gs_log(" pid:%lu, type:%u, shmid:%lu, key:%lu (%lx)",
			pp->pid,pp->type,pp->shmid,pp->key,pp->key);
	}
signal(SIGUSR1,gs_usr1);
alarm(oldalrm);
}




 /*------------------------------------------------------------------
 *
 *  Shutdown the Connection Manager & release its resources.
 *  If this is not done here, then the shared memory segments
 *  and semaphores we have allocated will stick around until
 *  the system is rebooted.   This causes other problems as
 *  well since ipc resources are extremely finite, so next time
 *  we connect, we will not be able to grab the memory we expect.
 *
 *------------------------------------------------------------------*/
static int gs_term_cm()
{
	int     status = GS_SUCCESS;
	int id;

	errno = 0;
	if (pcm->cmslot == (GSSlot_t *) 0) {
		gs_log("gs_delete_slot: Null slot pointer.");
		return (GS_FAIL);
	}
	if(pcm->debug)
		gs_log("cm:Deleting slot resources...");

	gs_sem_rm(pcm->cmslot->locksem);
	gs_sem_rm(pcm->cmslot->msgsem);
	gs_sem_rm(pcm->cmslot->cmsem);

	id = pcm->cmslot->id;

	if (gs_shm_free((char *)pcm->cmslot) != GS_SUCCESS) {
		gs_log("cm:gs_delete_slot:Cannot detach shared memory %d", pcm->cmslot);
		status = GS_FAIL;
	}
	else {
		shmctl(id, IPC_RMID, (struct shmid_ds *) 0);
	}

	return (status);
}



 /*
 *-------------------------------------------------------------
 * Create and initialize the connection manager datastructures
 * such as the shared memory message slot and its semaphores.
 * Also read the configuration file and configure as necessary
 *-------------------------------------------------------------
 */
static int gs_init_cm()
{
	int     status;
	char   *cp;

	cp = getenv("GSCMKEY");
	if (cp == (char *) 0) {
	/* No CM slot key Env variable defined. */
	/* Use the config file path instead. */
		pcm->cm_key = gs_ipc_key(pcm->config_file);
		if (pcm->cm_key->key == (key_t) - 1) {
			gs_log("Error creating key with ftok()");
			exit(1);
		}
	}
	else {
		pcm->cm_key->key = (key_t) atoi(cp);
	}

	status = gs_create_slot(pcm->cm_key, &(pcm->cmslot));
	if (status != GS_SUCCESS) {
		gs_log("gs_init_cm: Error creating slot.");
	}

	return (status);
}

static void usage()
{

	printf("/n Usage:\n");
	printf("	gs_cm [-d] [-f <config file>]\n");
	printf("    -d		- Debug mode. Run in foreground");
	printf("    -f file	- Config file spec.  (def:$GSHOME/gstab");
}


static int gs_cm_mainloop()
{
	int     done = FALSE;
	int     itr = 0;
	int     status = GS_SUCCESS;
	GSMessage_t rmsg;

	GsState = InitState; /* Startup - Init state. */
	errno = 0;
	itr = 0;
	cm_timeout = FALSE;
	alarm(GSIGORINTERVAL);
	signal(SIGALRM, gs_igor);
	signal(SIGHUP, gs_hup);
	while (!done) {
		itr++;
		do {
			errno = 0;
			if (cm_timeout == TRUE) {
				gs_log("cm: timeout.");
				done = TRUE;
				break;
			}

			alarm(GSIGORINTERVAL);
			signal(SIGALRM, gs_igor);
			GsState = WaitcmsemState;
			status = gs_sem_wait(pcm->cmslot->cmsem);
			if (status == GS_SUCCESS) {
				break;
			}
			else if (status == EINTR) {
				continue;
			}
			else {
				gs_log("cm:Sem error while waiting:%d", status);
				done = TRUE;
				break;
			}
		} while (1);

		signal(SIGALRM, SIG_IGN);
		if (done) {
			GsState = DoneState;
			do {
				sleep(0);
				errno = 0;
				gs_sem_signal(pcm->cmslot->msgsem);
			} while (errno == EINTR);

			sleep(0);
			break;
		}
		if (pcm->debug) {
			gs_sem_dump("  (CM)	locksem", pcm->cmslot->locksem);
			gs_sem_dump("  (CM)	msgsem", pcm->cmslot->msgsem);
			gs_sem_dump("  (CM)	cmsem", pcm->cmslot->cmsem);
		}


		if (pcm->cmslot->msg.MsgLen >= 0 &&
			pcm->cmslot->msg.MsgType == Executive &&
			pcm->cmslot->msg.MsgLen <= GSMAXMESSAGE) {
			rmsg.MsgType = 0;
			rmsg.MsgLen = 0;
			GsState = ProcessMsgState;
			status = gs_cm_process_msg(pcm,&pcm->cmslot->msg, &rmsg);
			if (status == GS_SUCCESS) {
				memcpy(&pcm->cmslot->msg, &rmsg, sizeof rmsg);
			}
			else {
				int *pint = (int *)&pcm->cmslot->msg.MsgBuf[0];
				pcm->cmslot->msg.MsgType = Status;
				pcm->cmslot->msg.MsgLen = sizeof status;
				*pint  = status;
			}
		}

		do {
			errno = 0;
			gs_sem_signal(pcm->cmslot->msgsem);
		} while (errno == EINTR);
		sleep(0);
	}
	return status;
}


/*ARGSUSED*/
void    main(argc, argv, envp)
int     argc;
char   *argv[], *envp[];
{
	int     ppid = 0;
	int     pid = 0;
	int     errflag = 0;
	char    c;
	char    text[256];
	char    pname[BUFSIZ];
	int     status = GS_SUCCESS;
	struct stat statbuf;
	static char *hptr;

	errno = 0;
	pcm->config_file[0] = '\0';
	pcm->debug = FALSE;
	pcm->homedir[0] = '\0';
	GsState = InitState;

	while ((c = getopt(argc, argv, "dh:f:")) != -1) {
		switch (c) {
		case 'd':
			pcm->debug = TRUE;
			break;

		case 'h':
			hptr=(char *)malloc(strlen(optarg)+16);
			sprintf(hptr,"GSHOME=%s",optarg);
			putenv (hptr);
			strcpy(pcm->homedir, optarg);
			break;

		case 'f':   /* turn on debugging */
			strcpy(pcm->config_file, optarg);
			break;

		case '?':   /* bad argument */
			errflag = TRUE;
			break;

		default:    /* this can't happen! */
			fprintf(stderr, "%s: BAD RETURN FROM GETOPT (%d)!\n", c);
			exit(1);
		}
	}
	if (errflag == TRUE) {
		usage();
		exit(1);
	}

	if (pcm->homedir[0] == '\0') {
		if (gs_home(pcm->homedir) == NULL) {
			fprintf(stderr, "gs_cm: GSHOME not defined.\n");
			exit(1);
		}
	}

	if (stat(pcm->homedir, &statbuf)) {
		fprintf(stderr, "Can't locate path: %s\n", pcm->homedir);
		exit(1);
	}
	if (gs_faccess(pcm->homedir, (char *) "rw", (char *) 0)) {
		fprintf(stderr, "Directory %s is not r/w accessible.\n", pcm->homedir);
		exit(1);
	}

	if (pcm->config_file[0] == '\0') {
		sprintf(pcm->config_file, "%s/gstab", pcm->homedir);
	}
	if (stat(pcm->config_file, &statbuf)) {
		sprintf(text, "%s/%s", pcm->homedir, pcm->config_file);
		strcpy(pcm->config_file, text);
		if (stat(pcm->config_file, &statbuf)) {
			fprintf(stderr, "Error!!! Cannot find configuration file: %s\n",
				pcm->config_file);
			exit(1);
		}
	}

	if (pcm->debug) {
		gs_log("Debug mode ON\n");
		gs_log("Config file is %s\n", pcm->config_file);
		gs_log("Home directory is %s\n", pcm->homedir);
	}


/*
 * Ignore terminal signals if defined (BSD only)
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
			gs_log("Cannot fork.");
		else if (pid > 0)
			exit(0);

#ifdef BSD		     /* BSD */
		if (setpgrp(0, getpid()) == -1)	/* Become leader of new proc. group */
			gs_log("Cant change process group");
	/* Close process terminal */
		if ((fd = open("/dev/tty", O_RDWR)) >= 0) {
			ioctl(fd, TIOCNOTTY, (char *) 0);
			close(fd);
		}
#else			     /* SYSV */
		if (setpgrp() == -1)
			gs_log("Can't change process group");
		signal(SIGHUP, SIG_IGN);	/* Let child survive kill of
						 * parent */
		if ((pid = fork()) < 0)	/* fork child again to disassociate */
			gs_log("Can't fork second child");
		else if (pid > 0)
			exit(0);
#endif
		/* Close descriptors in child process. */
		{
			int     f;
			for (f = 0; f < NOFILE; f++)
				(void) close(f);
		}

		umask(0);
		chdir("/"); 		/* cd to a local (real) filesystem. */
		errno = 0;
	}

/*
 * Handle child process exit notification.
 */
					/* If BSD or sun, then handle SIGCLD */
#if defined(BSD) || defined(sun)
	signal(SIGCLD, gs_cm_sigcld);
#else			     /* For SYSV, handler not necessary */
	signal(SIGCLD, SIG_IGN);
#endif

	pcm->cm_pid = pid;
	status = gs_init_cm();

	if (status != GS_SUCCESS) {
		gs_log("Failed to Initialize CM.");
		exit(1);
	}

	strcpy(pname, argv[0]);
	strncpy(argv[0], "Boris", strlen(argv[0]));

 /* Fork so we can close connection & cleanup even if CM dies */
	pid = fork();
	if (setjmp(gs_cmreapenv)) {
		gs_term_cm();
		kill(pid, SIGKILL);
		exit(0);
	/* NOTREACHED */
	}

	(void) signal(SIGTERM, gs_cmtermvec);
	(void) signal(SIGINT, gs_cmintvec);

 /* If parent goes away, cleanup and exit */
	if (pid == 0) {
		ppid = getppid();
#ifdef BSD		     /* BSD */
		setpgrp(0, getpid());	/* Become leader of new proc. */
#else
		setpgrp();
#endif
		while (1) {
			if (kill(ppid, 0) < 0) {
				errno = 0;
				gs_log("%s: cleaning up cm resources. (CM has died)",argv[0]);
				gs_term_cm();
				exit(0);
			}
			sleep(10);
		}
	}

	strcpy(argv[0], pname);
	signal(SIGUSR1,gs_usr1);
	pcm->cm_pid = getpid();
	cmplist.pid = pcm->cm_pid;
	cmplist.type = 1;
	cmplist.key = pcm->cm_key->key;
	cmplist.shmid = pcm->cmslot->id;
	cmplist.next = (proc *)0;
	gs_cm_mainloop();
	gs_term_cm();
	exit(0);
}

#endif			     /* SERVER */
