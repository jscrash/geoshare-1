#include "gs.h"
#define SYSV 1
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

#include <stdarg.h>

extern char *malloc();

/****************************************************************************
Module: GS_RECEIVE

Description:
        Receive a transfer control block that contains  either  a
transfer structure containing data or an executive structure con-
taining a protocol request.

Usage:
	status = gs_receive(handle, tcb);

Parameters:
	INPUT:
		GSHandle_t handle	- Connection handle.
		GSTCB_t *tcb		- Pointer to data tcb received.

	OUTPUT:
		Status of receive operation.

Status:

	This routine returns GS_SUCCESS upon normal successful completion.
	If an error occurrs, one of the following codes is returned:

	GSERR_MSGTYPE        - Unknown message type.

	GSERR_CM_NOTINIT     - The connection manager is not
			     - available, request is refused.


Author:
	Julian Carlisle 	 Fri Apr 19 13:32:19 PDT 1991
*****************************************************************************/

static int rec_timeout = FALSE;

static void gsr_alrm()
{
	rec_timeout = TRUE;
}

static char *pooladdr = NULL;
static int poolid = 0;


int     gs_receive(h, tcb)
GSHandle_t *h;
GSTCB_t **tcb;
{
	int     done = FALSE;
	int     itr = 0;
	int     rstatus,status = GS_SUCCESS;
	GSMessage_t rmsg;
	datamsg *dp;
	ptrmsg *pp;
	statusmsg *sp;
	int id;
	char *cp;
	struct stat statbuf;

	done = FALSE;
	rstatus = GS_SUCCESS;
	errno = 0;
	itr = 0;
	rec_timeout = FALSE;
	alarm(300);
	signal(SIGALRM, gsr_alrm);
	signal(SIGHUP, gsr_alrm);

	while (!done) {
		itr++;
		do {
			errno = 0;
			if (rec_timeout == TRUE) {
				gs_log("rec: timeout.");
				done = TRUE;
				break;
			}
			alarm(300);
			signal(SIGALRM, gsr_alrm);
			gs_semP(h->slot->cmsem);
			status = errno;
			if (status == 0) {
				break;
			}
			else if (status == EINTR) {
				continue;
			}
			else {
				gs_log("rec:Sem error while waiting:%d", status);
				done = TRUE;
				break;
			}
		} while (1);

		if (done) {
			do {
				sleep(0);
				errno = 0;
				gs_semV(h->slot->msgsem);
			} while (errno == EINTR);

			sleep(0);
			break;
		}
		sp = (statusmsg *)&rmsg.MsgBuf[0];
		sp->buf.uibuf[0] = GS_SUCCESS;
		rmsg.MsgType = Status;
		rmsg.MsgLen = 1;

		dp = (datamsg *)&h->slot->msg.MsgBuf[0];	

		switch(h->slot->msg.MsgType) {
		case Terminate:
			gs_log("rec:Terminate Msg received.");
			done = 1;
			rec_timeout = 1;
			sp = (statusmsg *)&h->slot->msg.MsgBuf[0];
			h->slot->msg.MsgType = Status;
			h->slot->msg.MsgLen = sizeof status;
			sp->buf.uibuf[0] = status;
			*tcb = (GSTCB_t *)0;
			rstatus = GS_TERMINATE;
			sleep(0);
			if (! stat(h->key->str,&statbuf)) {
				unlink(h->key->str);
			}
			break;

		case Data:
			switch(dp->class) {
			case String:
				dp->buf.cbuf[h->slot->msg.MsgLen] = '\0';
				if (h->slot->msg.MsgLen > 0) {
					gs_log("rec: Message: %s", dp->buf.cbuf);
				}
				else {
					gs_log("rec: MsgLen: %u", h->slot->msg.MsgLen);
					sp = (statusmsg *)&h->slot->msg.MsgBuf[0];
					h->slot->msg.MsgType = Status;
					h->slot->msg.MsgLen = sizeof status;
					sp->buf.uibuf[0] = status;
				}	
				break;

			case SharedPointer:
				pp = (ptrmsg *)&h->slot->msg.MsgBuf[0];	
				errno = 0;
				id = shmget(pp->key,pp->size,0);
				if (id > 0) {
					status = rstatus = GS_SUCCESS;
				}
				else {
					gs_log("rec: Error getting pool memory");
					status = rstatus = GS_FAIL;
					break;
				}
				if (status == GS_SUCCESS && poolid != id) {
					cp = (char *)shmat(id,pp->address,SHM_RDONLY);
					if (cp > (char *)0) {
					pooladdr =cp;
					}
					else {
						gs_log("rec: Error attaching shared memory.");
						status = rstatus = GS_FAIL;
						break;
					}
				}
				poolid = id;

				*tcb = (GSTCB_t *)pp->buf.uibuf[0];	
				done = TRUE;
				status = rstatus = GS_SUCCESS;
				break;

			case VectorStart:
				break;
			case VectorContinue:
				break;
			case VectorEnd:
				break;
			case Pointer:
				break;
			}
			break;

		default:
			gs_log("rec: MsgType: %u", h->slot->msg.MsgType);
			sp = (statusmsg *)&h->slot->msg.MsgBuf[0];
			h->slot->msg.MsgType = Status;
			h->slot->msg.MsgLen = sizeof status;
			sp->buf.uibuf[0] = status;
			rstatus = GSERR_MSGTYPE;
		}

		do {
			errno = 0;
			gs_semV(h->slot->msgsem);
			status = errno;
			if (status == EINVAL) {
				done = TRUE;
				status  = GS_FAIL;	
				break;
			}
			if (status != 0)
				gs_log("rec: error %d while signalling msgsem", errno);
		} while (status == EINTR);
		sleep(0);
	}
	return rstatus;
}
