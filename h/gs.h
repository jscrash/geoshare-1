#ifndef _GS_H_DEFINED
#define _GS_H_DEFINED
/*------------------------------------------------------------------------

Module:
        gs.h  -  GeoShare System Header File.


Description:
        This file contains all of the definitions required by
        GeoShare Application Programmer Interface library
        routines.

        Application programs only need to include this file
        to include all the required defintions for using the
        Geoshare library routines.


History:
       Mon Apr 8 03:21:53 PDT 1991 	- julian
            - Initial Version -

       Fri Apr 19 05:14:56 PDT 1991 	- julian
           Add definitions for process slots and messages.

------------------------------------------------------------------------*/
#ifndef ESI_GL_DEFS_H
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
/* typedef long key_t; */
#else
typedef long key_t;
extern int setjmp();
#endif
#include <errno.h>
#include "geoscience.h"
#include "gs_err.h"

#if 0
#ifdef __STDC__
   extern int abort();
   extern int alarm();
   extern int atoi();
   extern int close();
   extern int dup();
   extern int execv();
   extern void exit();
   extern int fclose();
   extern int fflush();
   extern int fork();
   extern int free();
   extern int ftok();
   extern int getopt();
   extern int getpid();
   extern int getppid();
   extern char *memcpy();
   extern char *mktemp();
   extern int  putenv();
   extern int read();
   extern char *realloc();
   extern void setbuf();
   extern int setgid();
   extern int setpgrp();
   extern int setuid();
   extern char *shmat();
   extern int shmctl();
   extern int shmdt();
   extern int shmget();
   extern void sleep();
   extern int system();
   extern mode_t umask();
   extern int unlink();
   extern char *memccpy();
   extern int vfprintf();
   extern int fprintf();
   extern int sprintf();
   extern int printf();
#endif
#endif

#define numeric(c)		(c >= '0' && c <= '9')
#define max(a,b) 		(a<b ? b : a)
#define min(a,b) 		(a>b ? b : a)
#define abs(x)			(x>=0 ? x : -(x))

#define copy(srce,dest)		cat(dest,srce,0)
#define compare(str1,str2)	strcmp(str1,str2)
#define equal(str1,str2)	!strcmp(str1,str2)
#define length(str)		strlen(str)

 /*
  * Use: Debug(sum,d) which becomes fprintf(stderr,"sum = %d\n",sum)
  * Remember: The connection manager process has no standard error 
  *  so don't use this macro in that routine.
  */
#define Debug(variable,type)	fprintf(stderr,"variable = %type\n",variable)

#define NONBLANK(p)		while (*p==' ' || *p=='\t') p++

 /*
  * A global null string.
  */
extern char Null[1];

 /*
  * A global error message string.
  */
extern char Error[128];

#ifndef TRUE
#   define TRUE            1
#endif

#ifndef FALSE
#   define FALSE           0
#endif

#define GSLOGFILENAME   "gscm_messages.log"
#define GS_TERMINATE	254
#define RETRYS          4
#define MAXTRYS 	4
#define GSSLOTBUFSIZE   32768
#define GSMAXMESSAGE    16384
#define GSMAXKEY        64
#define GSDEFAULTHOME 	"/etc"

#ifndef SUCCESS
#   define SUCCESS         ((long)0)
#endif

#ifndef FAIL
#   define FAIL            ((long)-1)
#endif

#ifndef EXIT_SUCCESS
#   ifdef unix
#      define EXIT_SUCCESS ((long)1)
#   else
#      define EXIT_SUCCESS ((long)0)
#   endif
#endif

#ifndef EXIT_FAIL
#   ifdef unix
#      define EXIT_FAIL ((long)0)
#   else
#      define EXIT_FAIL ((long)-1)
#   endif
#endif

#define GS_SUCCESS      SUCCESS	      /* Successful Completion      */
#define GS_FAIL         FAIL          /* Operation Failed           */

#define GS_EXIT_SUCCESS EXIT_SUCCESS  /* main Exit code for Success */
#define GS_EXIT_FAIL    EXIT_FAIL     /* main Exit code for failure */

 /*
  * Send Error string to Connection Manager if handle is valid. Otherwise
  * just write the string to the local error log in the GSHOME directory.
  */

void    Error_Handler();
void    Process_executive();

#define FTOKID ('a')	     /* Project code for cm & app sem/shm keys */

extern int errno;
extern char *getcwd(), *getenv();
#if 0
extern char *index(), *malloc();
#endif
/*
 * Message Definitions
 */
typedef enum {
	Undefined = 0,
	Data = 1,
	Executive = 2,
	Status = 3,
	Terminate = 4,
	ReceiverInq = 5
} gs_msgtype_t;

 /*
 * Classes of Data messages
 */
 typedef enum {
	String  = 64,
	Pointer,
	SharedPointer,
	VectorStart,
	VectorContinue,
	VectorEnd
	} gs_dataclass;
/*
 * Classes of Executive Request.
 */
typedef enum {
	InqAllReceivers = 128,
	InqCmStatus,
	InqReceiverArgs,
	InqReceiverMaxMem,
	InqReceiverStatus,
	SetReceiverArgs,
	SetReceiverState,
	StartReceiver,
	StopReceiver
}       gs_exectypes;



typedef struct pent {
	int pid;		/* pid of this process entry */
	int type;		/* type of process: APP or REC */
	key_t key;		/* key of slot owned by process if type=APP */
	int shmid;		/* shm id of slot owned by process if type=APP */
	struct pent *next;	/* next process entry in list. */
	} proc,*pproc;



typedef struct gs_message {
	unsigned int MsgType;
	unsigned int MsgLen;
	unsigned int pid;
	unsigned int shmid;
	key_t key;
	unsigned char MsgBuf[GSMAXMESSAGE+64];
} GSMessage_t;


typedef struct gs_datamsg{
	gs_dataclass class;
	union {
		unsigned char 	cbuf[GSMAXMESSAGE];
		unsigned int 	uibuf[GSMAXMESSAGE/4];
		int 		ibuf[GSMAXMESSAGE/4];
	} buf;
	char end_of_data;
} datamsg;

typedef struct gs_ptrmsg {
	gs_dataclass class;
	key_t key;
	char *address;
	unsigned int id;
	unsigned int size;

	union {
		unsigned char 	cbuf[GSMAXMESSAGE];
		unsigned int 	uibuf[GSMAXMESSAGE/4];
		int 		ibuf[GSMAXMESSAGE/4];
	} buf;
	char end_of_data;
} ptrmsg;

typedef struct gs_statusmsg{
	union {
		unsigned char 	cbuf[GSMAXMESSAGE];
		unsigned int 	uibuf[GSMAXMESSAGE/4];
		int 		ibuf[GSMAXMESSAGE/4];
	} buf;
	char end_of_status;
} statusmsg;


typedef struct {
	int     id;
	key_t   slotkey;
	key_t   lockkey;
	key_t   msgkey;
	key_t   cmkey;
	int     locksem;
	int     msgsem;
	int     cmsem;
	GSMessage_t msg;
} GSSlot_t;

typedef struct {
	int     id;
	key_t   slotkey;
	key_t   lockkey;
	key_t   msgkey;
	key_t   cmkey;
	int     locksem;
	int     msgsem;
	int     cmsem;
	GSMessage_t msg;
}       CMSlot_t;

/* Shared memory identifier key */
typedef struct gs_key {
	int     type;
	key_t   key;
	char   *str;
	char key_ext;
} GSKey_t;

typedef struct {
	int     debug;
	int     cm_pid;
	int     cm_shmid;
	GSKey_t *cm_key;
	GSSlot_t *cmslot;
	FILE   *config_fp;
	char    config_file[128];
	char    pgm_name[128];
	char    homedir[128];
} GSGBL_t;


/*
 * Communications Handle Definition
 */
typedef struct {
	int     init;		/* handl init state. Must be TRUE to use */
	int     debug;		/* if =1 then log messages verbosly */	
	int     pid;		/* Application program PID */
	int     cmpid;		/* PID of connection manager */	
	int     shmid;		/* shm id for CM slot shared mem segment */
	char    cfgfile[128];	/* path+filename of gstab file */
	char    pgmname[128];
	char    homedir[128];	/* GSHOME directory */
	char    logfile[128];	/* Log file written to by gs_log()	*/
	GSKey_t *key;		/* ipc key for creating/attaching slot */	
	GSKey_t *cmkey;		/* ipc key for creating/attaching cmslot */	
	GSSlot_t *slot;		/* pointer to slot shared memory segment */
	GSSlot_t *cmslot;	/* pointer to cmslot shared memory segment */
	FILE   *config_fp;
	int   status;		/* Status of last operation using handle */
} GSHandle_t;

/*
 *Transfer Control Block Structure
 */
typedef struct tcb_t {
	char    id[4];
	int     BlockNum;
	int     LastBlock;
	GSStructure_t data;
} GSTCB_t;


/*
 * GeoShare API library prototypes.
 *
 * Julian 	April 16,1991
*/
#if 0
#include <varargs.h>
#include "gs_ptypes.h"
#endif
#endif
#include <stdarg.h>
