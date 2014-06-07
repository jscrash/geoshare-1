#include "gs.h"
#include <string.h>
#include "errno.h"
#include <signal.h>



/****************************************************************************
Module: GS_TERMINATE

Description:
	This routine will perform a graceful shutdown of the connection
	indicated by the handle argument.  All buffers are flushed, and the
	receiver process is shutdown.  The final status of the receiver
	program is returned as the value of this function.

Usage:
	status = gs_terminate(handle);
	
Parameters:
	INPUT:
		GSHandle_t handle	- Connection to terminate.

	OUTPUT:
		Status of receiver is returned as value of this function

Status:
	If an error occurrs while terminating the receiver, one of the
	following error codes may be returned:

	GSERR_SVC_UNAVAIL    - Unknown data service.

	GSERR_CM_NOTINIT     - The connection manager is not
			     - available, request is refused.


Author:
	Julian Carlisle 	 Fri Apr 19 15:41:53 PDT 1991
*****************************************************************************/

int     gs_terminate(h)
GSHandle_t *h;
{
	int     status = GS_SUCCESS;
	GSMessage_t msg, reply;

	if (h == (GSHandle_t *)0) {
		gs_log("gs_terminate: Null Handle passed.");
		return (GS_FAIL);
	}
	if (h->slot == (GSSlot_t *)0) {
		gs_log("gs_terminate: Null Slot passed.");
		return (GS_FAIL);
	}
	msg.MsgType = Terminate;
	msg.MsgLen = 0;
	reply.MsgBuf[0] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */
	status = gs_rec_talk(h, h->slot, &msg, &reply);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_terminate: rec_talk status = %d\n", status);
	}

	sleep (0);
	if (h->debug) {
		gs_log("gs_terminate: Deleting receiver slot. %d %d",h->slot,h->cmslot);
	}
	if (gsz_delete_slot(h->slot) != GS_SUCCESS)
		if (h->debug)
			gs_log("gs_terminate: Error deleting receiver slot.");
	h->slot = (GSSlot_t *)0;

	return status;
}
