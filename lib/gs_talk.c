#include "gs.h"
#include <errno.h>

/****************************************************************************
Module:	GS_TALK

Description:
	Perform synchronized message transactions with the connection
	manager.  This routine performs the task of sending messages to the
	connection manager via its message slot identified by a well known
	address.  Synchronous access to the CM message buffer is acheived by
	implementing a semaphore driven protocol.
	
	A message may optionally wait for a reply before returning.

Usage:	gs_talk(handle,msg,&reply);

	
Parameters:
	INPUT:
		GSHandle_t handle	- Connection descriptor
		GSMessage_t msg	- Message object

	OUTPUT:
		GSMessage_t *reply	- Addres of pointer for reply msg.

Status:
	This routine returns GS_SUCCESS upon successful completion of the
	message transaction.  If an error occurrs during the transaction one
	of the following error codes will be returned:

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
	Julian Carlisle 	 Fri Apr 20 03:12:45 PDT 1991
*****************************************************************************/

extern int errno;

int     gs_talk(h, pslot, msg, reply)
GSHandle_t *h;
GSSlot_t *pslot;
GSMessage_t *msg;
GSMessage_t *reply;
{
	int     status = GS_SUCCESS;
	int     semstat = 0;
	errno = 0;

	if (h == (GSHandle_t *) 0) {
		gs_log("gs_talk: Null handle.");
		return (GS_FAIL);
	}

	if (h->init != TRUE) {
		gs_log("gs_talk: Handle not initialized.");
		return (GS_FAIL);
	}

 /* Check the message length */
	if (msg->MsgLen > GSMAXMESSAGE) {
		gs_log("gs_talk: Message is too long.");
		return (GS_FAIL);
	}

 /* wait for the Message Lock semaphore */
 /* if semaphore comes back indicating that it was interrupted */
 /* then repeat the operation. */
	do {
		errno = 0;
		semstat = gs_sem_wait(pslot->locksem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:1 error waiting on locksem %d\n", semstat);
		}
	} while (semstat == EINTR);

	do {
		errno = 0;
		semstat = gs_sem_wait(pslot->msgsem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:2 error waiting on msgsem %d\n", semstat);
		}
	} while (semstat == EINTR);


	pslot->msg.MsgType = msg->MsgType;
	pslot->msg.MsgLen = msg->MsgLen;
	memcpy(pslot->msg.MsgBuf, msg->MsgBuf, msg->MsgLen);
	pslot->msg.pid = msg->pid;
	pslot->msg.shmid = msg->shmid;
	pslot->msg.key = msg->key;

 /* signal CM to read message */
	do {
		errno = 0;
		semstat = gs_sem_signal(pslot->cmsem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:3 error signalling  on cmsem %d\n", semstat);
		}
	} while (semstat == EINTR);

#if 0
	while (pslot->msg.MsgType == msg->MsgType) {
		sleep(0);
	}
#else
	sleep(0);
#endif

 /* wait for the cm semaphore and lock it */
 /* if semaphore comes back indicating that it was interrupted */
 /* then repeat the operation. */
	do {
		errno = 0;
		semstat = gs_sem_wait(pslot->msgsem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:4 error waiting on msgsem %d\n", semstat);
		}
	} while (semstat == EINTR);

 /* read reply */
	if (pslot->msg.MsgType == 0) {
		gs_log("gs_talk: Received Bad Message from cm.");
		status = GS_FAIL;
	}
	else {
		reply->MsgType = pslot->msg.MsgType;
		reply->MsgLen = pslot->msg.MsgLen;
		memcpy(reply->MsgBuf, pslot->msg.MsgBuf, pslot->msg.MsgLen);
		reply->MsgBuf[reply->MsgLen] = '\0';
		pslot->msg.MsgType = 0;
		pslot->msg.MsgLen = 0;
		pslot->msg.MsgBuf[0] = '\0';
		pslot->msg.pid = 0;
		pslot->msg.shmid = 0;
		pslot->msg.key = (key_t)0;
		status = GS_SUCCESS;
	}
 /* free the slot for other procs */
	do {
		errno = 0;
		semstat = gs_sem_signal(pslot->msgsem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:5 error signalling msgsem %d\n", semstat);
		}
	} while (semstat == EINTR);
	do {
		errno = 0;
		semstat = gs_sem_signal(pslot->locksem);
		if (semstat != GS_SUCCESS) {
			if (semstat != EINTR)
				fprintf(stderr,"talk:6 error signalling locksem %d\n", semstat);
		}
		sleep(0);
	} while (semstat == EINTR);

	return (status);
}

 /*
  * This is a version of talk that is used to talk to the receiver slot using
  * the binary semaphores instead of the cluster semaphores.   This is the
  * protocol complement to gs_receive.
  * 
  * Julian	 Tue May  7 01:10:42 PDT 1991
  */
int     gs_rec_talk(h, pslot, msg, reply)
GSHandle_t *h;
GSSlot_t *pslot;
GSMessage_t *msg;
GSMessage_t *reply;
{
	int     status = GS_SUCCESS;

	errno = 0;

	if (h == (GSHandle_t *) 0) {
		gs_log("gs_rec_talk: Null handle.");
		return (GS_FAIL);
	}

	if (h->init != TRUE) {
		gs_log("gs_rec_talk: Handle not initialized.");
		return (GS_FAIL);
	}

 /* Check the message length */
	if (msg->MsgLen > GSMAXMESSAGE) {
		gs_log("gs_rec_talk: Message is too long.");
		return (GS_FAIL);
	}

 /* Get the send semaphore */
	do {
		errno = 0;
		gs_semP(pslot->msgsem);
		status = errno;
		if (errno != 0)
			fprintf(stderr,"rec_talk:2 error waiting on msgsem %d\n", errno);
	} while (status == EINTR);

 /* write message to slot */
	pslot->msg.MsgType = msg->MsgType;
	pslot->msg.MsgLen = msg->MsgLen;
	memcpy(pslot->msg.MsgBuf, msg->MsgBuf, msg->MsgLen+64);

 /* signal partner to read message */
	do {
		errno = 0;
		gs_semV(pslot->cmsem);
		status = errno;
		if (errno != 0)
			fprintf(stderr,"rec_talk:3 error signalling  on cmsem %d\n", errno);
	} while (status == EINTR);

	sleep(0);

	do {
		errno = 0;
		gs_semP(pslot->msgsem);
		status = errno;
		if (errno != 0)
			fprintf(stderr,"rec_talk:4 error waiting on msgsem %d\n", errno);
	} while (status == EINTR);

 /* read reply */
	if (pslot->msg.MsgType == 0) {
		gs_log("gs_rec_talk: Received Bad Message from cm.");
		status = GS_FAIL;
	}
	else {
		reply->MsgType = pslot->msg.MsgType;
		reply->MsgLen = pslot->msg.MsgLen;
		memcpy(reply->MsgBuf, pslot->msg.MsgBuf, pslot->msg.MsgLen+64);
		pslot->msg.MsgType = 0;
		pslot->msg.MsgLen = 0;
		pslot->msg.MsgBuf[0] = '\0';
		status = GS_SUCCESS;
	}
 /* free the slot for other procs */

	do {
		errno = 0;
		gs_semV(pslot->msgsem);
		if (errno != 0)
			fprintf(stderr,"rec_talk:5 error signalling msgsem %d\n", errno);
	} while (errno == EINTR);

	sleep(0);

	return (status);
}
