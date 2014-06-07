#include "gs.h"
#include <string.h>

/****************************************************************************
Module: GS_SEND

Description:
	This routine transfers data stored in GSTCB_t format to the
	destination indicated by the handle argument.  After successfully
	transferring the data, a result structure is returned that indicates
	the final disposition of the transfer.

Usage:
	status = gs_send(handle,tcb,&result);

Parameters:
	INPUT:
		GSHandle_t handle	- Initialized communications handle
		GSTCB_t  tcb		- tcb containing data to send

	OUTPUT:
		GSResult_t *result	- Result/status of transfer.

Status:
	This routine will return GS_SUCCESS upon successful completion of
	the data transfer.  If an error occurrs, one of the following may be
	returned:

                GSERR_MSGTYPE        - Unknown message type.

                GSERR_EXECTYPE       - Unknown Executive Request.

                GSERR_SVC_BUSY       - Requested data receiver is single
                                     - threaded, and already in use.

                GSERR_SVC_MISMATCH   - The data type sent to the data.
                                     - service is not supported.

                GSERR_CM_NOTINIT     - The connection manager is not
                                     - available, request is refused.

Author:
	Julian Carlisle 	 Fri Apr 19 14:21:14 PDT 1991
*****************************************************************************/

extern key_t gs_poolkey;
extern char *gs_poolhead;
extern int gs_poolid;
extern int gs_poolsize;

int     gs_send(h, tcb, result)
GSHandle_t *h;
GSTCB_t *tcb;
GSMessage_t *result;
{
	int     status = GS_SUCCESS;
	GSMessage_t msg;
	statusmsg *sp;
	ptrmsg *pp;

	pp = (ptrmsg *)&msg.MsgBuf[0];
	msg.MsgType = Data;
	pp->class = SharedPointer;
	pp->key = gs_poolkey;
	pp->address = gs_poolhead;
	pp->id = gs_poolid;
	pp->size = gs_poolsize;
	pp->buf.uibuf[0] = (unsigned int)tcb;
	msg.MsgLen = sizeof(GSTCB_t *);

	result->MsgType = 0;
	result->MsgLen = 0;
	result->MsgBuf[0] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */

	status = gs_rec_talk(h, h->slot, &msg, result);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_send: rec_talk status = %d\n", status);

		/*return (status);*/
	}


 /*
  * handle the various possible replies from the Connection Manager
  */
	switch (result->MsgType) {
	case Status:
		sp = (statusmsg *)&result->MsgBuf[0];
		status = sp->buf.uibuf[0];
		if (h->debug)
			gs_log("gs_send: Reply is Status %d\n", status);
		break;

	default:
		if (h->debug)
			gs_log("gs_send: Msg Type:%d  Len:%d .\n",
				result->MsgType, result->MsgLen);
		status = GS_FAIL;
		break;
	}


 /*
  * Free up the temporary connection context we have established.
  */
	return (status);
}
