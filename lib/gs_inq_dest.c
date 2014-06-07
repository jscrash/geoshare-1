#include "gs.h"
#include <string.h>
#include "errno.h"

/****************************************************************************
Module: GS_INQ_DEST

Description:
	Get a list of all the data receiver services available through
	the local GeoShare connection manager.  The connection manager is
	queried via an Executive request, and returns the list of currently
	installed and available data receiversa in a NULL terminated array
	of character strings.  The data receivers are registered in the
	connection manager configuration file 'gstab'.

Usage:
	char **tcp;
	unsigned int count;
	status = gs_inquire_destinations(&tcp,&count);
	
Parameters:
	INPUT:
		none

	OUTPUT:
		char **tcp			- Allocated table of character strings.
		unsigned int count	-	Total destinations returned.	

Status:
	If the request is successful, GS_SUCCESS is returned.  If an error
	is encountered, one of the following status codes may be returned:

	GS_FAIL	             - Cannot queue request. (No Resources, etc)
	GSERR_MSGTYPE        - Unknown message type.
	GSERR_NOCONNECT      - Connect attempt failed.
	GSERR_EXECTYPE       - Unknown Executive Request.

	GSERR_CM_NOTINIT     - The connection manager is not
			     - available, request is refused.


Author:
	Julian Carlisle 	 Mon Apr 29 23:52:01 PDT 1991
*****************************************************************************/
extern int errno;



int     gs_inquire_destinations(dest_list,count)
char ***dest_list;
unsigned int *count;
{
	int     status = GS_SUCCESS;
	GSHandle_t *h = (GSHandle_t *) 0;
	GSMessage_t msg, reply;
	int     i;
	char   *cp = NULL;
	char  **tcp = (char **) 0;

	*count = 0;
	h = gs_get_handle();
	if (h == (GSHandle_t *) 0) {
		gs_log("gs_inquire_destinations: null handle from gs_get_handle\n");
		return (GSERR_CM_NOTINIT);
	}
#if 0
	if (h->debug) {
		gs_sem_dump("(inq dest) locksem", h->cmslot->locksem);
		gs_sem_dump("(inq dest) msgsem", h->cmslot->msgsem);
		gs_sem_dump("(inq dest) cmsem", h->cmslot->cmsem);
	}
#endif
	msg.MsgType = Executive;
	msg.MsgBuf[0] = InqAllReceivers;
	msg.MsgLen = 1;

	reply.MsgType = 0;
	reply.MsgLen = 0;
	reply.MsgBuf[0] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */
	status = gs_talk(h, h->cmslot, &msg, &reply);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_inquire_destinations: talk status=%d - detaching slot\n", status);

		if (gs_detach_slot(h->cmslot) == GS_SUCCESS)
			h->init = FALSE;
		else
			gs_log("gs_inquire_destinations: error detaching slot\n");

		return (status);
	}


 /*
  * handle the various possible replies from the Connection Manager
  */
	switch (reply.MsgType) {
	case Status:
		status = (int) reply.MsgBuf[0];
		if (h->debug)
			gs_log("gs_inquire_destinations: Reply is Status %d\n", status);
		break;

	case Data:
		if (h->debug)
			gs_log("gs_inquire_destinations: Message is %s\n", reply.MsgBuf);

		if (reply.MsgLen <= 1) {
			if (h->debug)
				gs_log("gs_inquire_destinations: No entries returned.");
			status = GS_FAIL;
			break;
		}

		cp = strtok(reply.MsgBuf, " ");
		for (i = 0; cp != NULL; cp = strtok(NULL, " "), i++) {
			gs_tcp_append(&tcp, cp);
		}

		status = GS_SUCCESS;
		*dest_list = tcp;
		*count = i;
		break;

	default:
		if (h->debug)
			gs_log("gs_inquire_destinations: Msg Type:%d  Len:%d .\n",
				reply.MsgType, reply.MsgLen);
		status = GSERR_MSGTYPE;
		*count = 0;
		break;
	}


 /*
  * Free up the temporary connection context we have established.
  */
	if (gs_detach_slot(h->cmslot) == GS_SUCCESS)
		h->init = FALSE;
	else
		gs_log("gs_inquire_destinations: error detaching slot\n");

	return (status);
}
