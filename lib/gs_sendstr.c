#include "gs.h"
#include <string.h>
#include "errno.h"

/****************************************************************************
Module: GS_SENDSTR

Description:
	Send a string  to the receiver process.  The string will be logged
	by the receiver to its message file.  This is primarily used to
	debug receivers or to write informational messages into the receiver
	message files, perhaps for marking events.

	Note that strings received by the receiver are processed internally and 
	do not cause a return from the gs_receive routine.

Usage:
	status = gs_sendstr(handle,string);

Parameters:
	INPUT:
		GSHandle_t handle	- Initialized communications handle
		char *string		- String to send to receiver.

	OUTPUT:
			NONE

Status:
	This routine will return GS_SUCCESS upon successful completion of
	the string transfer.  If an error occurrs, one of the following may be
	returned:
				GS_FAIL

                GSERR_MSGTYPE        - Unknown message type.

                GSERR_CM_NOTINIT     - The connection manager is not
                                     - available, request is refused.

Author:
	Julian Carlisle		Sun May 12 22:03:14 PDT 1991
***************************************************************************/

int     gs_sendstr(h, str)
GSHandle_t *h;
char   *str;
{
	int     status = GS_SUCCESS;
	GSMessage_t msg, reply;
	statusmsg *sp;
	datamsg *dp;

	dp = (datamsg *)&msg.MsgBuf[0];
	msg.MsgType = Data;
	dp->class = String;
	strcpy(dp->buf.cbuf, str);
	msg.MsgLen = strlen(str);

	reply.MsgType = 0;
	reply.MsgLen = 0;
	reply.MsgBuf[0] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */

	status = gs_rec_talk(h, h->slot, &msg, &reply);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_sendstr: rec_talk status = %d\n", status);

		/*return (status);*/
	}


 /*
  * handle the various possible replies from the Connection Manager
  */
	switch (reply.MsgType) {
	case Status:
		sp = (statusmsg *)&reply.MsgBuf[0];
		status = sp->buf.uibuf[0];
		if (h->debug)
			gs_log("gs_sendstr: Reply is Status %d\n", status);
		break;

	case Data:
		dp = (datamsg *)&reply.MsgBuf[0];
		if (h->debug)
			gs_log("gs_sendstr: Message is %s\n", dp->buf.cbuf);

		status = GS_SUCCESS;
		break;

	default:
		if (h->debug)
			gs_log("gs_sendstr: Msg Type:%d  Len:%d .\n",
				reply.MsgType, reply.MsgLen);
		status = GS_FAIL;
		break;
	}


 /*
  * Free up the temporary connection context we have established.
  */
	return (status);
}
