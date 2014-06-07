#define SYSV 1
#include "gs.h"
#include <fcntl.h>


/****************************************************************************
Module: GS_CM_DO_EXEC

Description:
	Do executive command processing for connection manager.

Usage:
	This is a CM internal routine, it is called as follows:

	GSMessage_t msg,reply;
	status = gs_cm_do_exec(&msg,&reply);

Parameters:
	INPUT:
		GSMessage_t 	msg	- Message from slot
		GSMessage_t     reply	- Reply from cmd processor

	OUTPUT:
		None.
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
	Julian Carlisle 	 Wed Apr 24 18:54:40 PDT 1991
*****************************************************************************/
#include  <string.h>
#include  <sys/stat.h>
#include  <sys/param.h>
#include  <sys/ipc.h>
#include <stdarg.h>
#include "math.h"


extern int errno;
extern char *getcwd(), *getenv();
extern char *index(), *malloc();

static char cfbuf[2048];
static char *cfptr;

int     gs_cm_process_msg(pcm,pmsg, preplymsg)
GSGBL_t *pcm;
GSMessage_t *pmsg, *preplymsg;
{
	int     status = GS_SUCCESS;
	int     cfd[3];
	char    arglist[256];
	char    text[512];
	char    recname[MAXPATHLEN];
	char    bindir[MAXPATHLEN];
	char    argbuf[256];
	char   *prec = NULL;
	char   *cp = NULL;
	int     i = 0;
	int     pid = 0,*pint;
	char  **tcp = (char **) 0;
	struct stat statbuf;

	pint = (int *)&preplymsg->MsgBuf[0];
	cfd[0] = 0;
	cfd[1] = 0;
	cfd[2] = 0;

	prec = recname;
	cfptr = (char *) &cfbuf[0];

	switch (pmsg->MsgBuf[0]) {
	case InqAllReceivers:
		status = gs_cfgstr("cm","all",(char *)preplymsg->MsgBuf,
			(int *)&preplymsg->MsgLen);
		preplymsg->MsgType = Data;
		break;

	case InqReceiverArgs:
		pmsg->MsgBuf[pmsg->MsgLen] = '\0';
		cp = (char *) &pmsg->MsgBuf[1];		
		status = gs_cfgstr(cp,"context",(char *)preplymsg->MsgBuf,
			(int *)&preplymsg->MsgLen);
		preplymsg->MsgType = Data;
		break;

	case StartReceiver:
		cp = (char *) &pmsg->MsgBuf[1];
		cp[pmsg->MsgLen] = '\0';
		prec = recname;
		while (*cp != ':') {
			*prec++ = *cp++;
		}
		*prec = '\0';
		gs_tcp_append(&tcp, recname);
		cp++;
		strcpy(arglist, cp);
		strcpy(argbuf, cp);
		cp = strtok(arglist, " ");
		for (i = 0; cp != NULL; cp = strtok(NULL, " "), i++) {
			gs_tcp_append(&tcp, cp);
		}
		status = gs_cfgstr("cm","bindir",bindir,&i);
		if (status != GS_SUCCESS) {
			gs_log("cm:warning: No bindir entry found in gstab. Using GSHOME");
			gs_home(bindir);
		}

		if (stat(bindir, &statbuf)) {
			gs_log("Directory %s not found or is inaccessible", cp);
			gs_status_msg(preplymsg,GSERR_SVC_UNAVAIL);
			status = GS_FAIL;
			break;
		}

		status = gs_cfgstr(recname,"program",text,&i);
		if (status != GS_SUCCESS) {
			gs_log("cm:no such receiver: (%s) ", recname);
			gs_status_msg(preplymsg,status);
			status = GS_FAIL;
			break;
		}
		sprintf(recname, "%s/%s", bindir, text);
		if (stat(recname, &statbuf)) {
			gs_log("cm: Cannot find receiver program: %s", recname);
			gs_status_msg(preplymsg,GSERR_SVC_UNAVAIL);
			status = GS_FAIL;
			break;
		}
		sprintf(text, "%s %s", recname, argbuf);
		if (pcm->debug) 
			gs_log("cm: %s", text);

		if ((pid = fork()) == -1) {
			gs_log("cm:Cannot create receiver process: %s", text);
			status = GS_FAIL;
			gs_status_msg(preplymsg,status);
			break;
		}
		else if (pid == 0) {
			cfd[1] = open("/tmp/rec.out", O_WRONLY | O_APPEND | O_CREAT, 0666);
			close(1);
			dup(cfd[1]);
			close(2);
			dup(cfd[1]);
			close(0);
/*		    dup(cfd[1]); */
			for (i = 3; i < 20; i++)
				close(i);
			signal(SIGALRM, SIG_IGN);
			signal(SIGHUP, SIG_IGN);
			alarm(0);
			execv(recname, tcp);
			gs_log("Exec fail. %s ", recname);
			exit(1);
		}
		else {
			if (pmsg->pid != 0) {
				gs_addproc(pmsg->pid,0,pmsg->shmid,pmsg->key);
			}
			if (pcm->debug)
				gs_log("cm: child process is %d", pid);
			gs_status_msg(preplymsg,pid);
			status = GS_SUCCESS;
		}
		break;

	case Executive:
		status = gs_do_exec(pmsg, preplymsg);
		break;

	default:
		status = GS_FAIL;
		gs_status_msg(preplymsg,status);
		gs_log("cm: Bad Executive request sent.");
		break;
	}
	return (status);
}

int     gs_do_exec(pmsg, preply)
GSMessage_t *pmsg, *preply;
{
	char    text[GSMAXMESSAGE];


	memcpy(text, pmsg->MsgBuf, pmsg->MsgLen);
	text[pmsg->MsgLen] = '\0';
	gs_log("cm: Message is %s\n", text);

	preply->MsgType = Status;
	preply->MsgLen = sizeof(int);
	preply->MsgBuf[0] = (int) GS_SUCCESS;

	return GS_SUCCESS;
}

int gs_cfgstr(entry,item,outbuf,outlen)
char *entry,*item,*outbuf;
int *outlen;
{
	int status = GS_SUCCESS;
	char ebuf[256];
	char buf[2048];
	char *bufp = buf,*cp;

	if (outbuf == NULL || outlen == (int *)0 ||
	    entry == NULL || item == NULL )
		return (GS_FAIL);

	*outlen = 0;
	*outbuf = '\0';

	strcpy(ebuf,entry);
		
	status = gs_cgetent(buf, ebuf);
	switch(status) {
	case 0:		/* not found */
		status =GSERR_SVC_UNAVAIL;
		break;

	case 1:		/* found */
		cp = gs_cgetstr(item,&bufp);
		if (cp == NULL) {
			status = GSERR_OPT_UNAVAIL;
		}
		else {
			*outlen = strlen(cp);
			strcpy(outbuf,cp);
			status = GS_SUCCESS;
		}
		break;
	
	default:
		status = GS_FAIL ;
		break;
	}
	return (status);
}


int gs_cfgnum(entry,item,result)
char *entry,*item;
int *result;
{
	int status = GS_SUCCESS;
	char ebuf[256];
	char buf[2048];
	int n = 0;

	if (result == (int *)0 ||  entry == NULL || item == NULL )
		return (GS_FAIL);

	*result = 0;

	strcpy(ebuf,entry);
		
	status = gs_cgetent(buf, ebuf);
	switch(status) {
	case 0:		/* not found */
		status =GSERR_SVC_UNAVAIL;
		break;

	case 1:		/* found */
		n = gs_cgetnum(item);
		if (n == -1) {
			status = GSERR_OPT_UNAVAIL;
		}
		else {
			*result = n;
			status = GS_SUCCESS;
		}
		break;
	
	default:
		status = GS_FAIL ;
		break;
	}
	return (status);
}

int gs_status_msg(pmsg,status)
GSMessage_t *pmsg;
int status;
{
register int *pint;

	if (pmsg == (GSMessage_t *)0) 
		return GS_FAIL;

	pint = (int *)&pmsg->MsgBuf[0];
	pmsg->MsgType = Status;
	pmsg->MsgLen = sizeof(int);
	*pint = status;
	
	return (GS_SUCCESS);
}


int gs_data_msg(pmsg,data,len)
GSMessage_t *pmsg;
char *data;
int len;
{
	register char *pc;

	if (data == NULL) 
		return (GS_FAIL);

	if (len == 0)
		len = GSMAXMESSAGE;

	pc = (char *)memccpy(pmsg->MsgBuf,data,'\0',len);
	pmsg->MsgType = Data;
	pmsg->MsgLen = (pc == NULL) ? len:(int)((pc-(char *)pmsg->MsgBuf)+(char)1);

	return GS_SUCCESS;		
}	
