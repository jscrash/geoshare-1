#include "gs.h"
#include <string.h>
#include "errno.h"

extern int errno;

/****************************************************************************
Module: GS_INQ_CTXT

Description:
	Get the list of arguments required for a given receiver service.
	The returned data structure contains the following for each required
	parameter:
		
	    * A string containing the command line text of the parameter,
	      i.e. '-project'

	    * A string containing the prompt for the user-interface that
	      collects the value portion of this parameter.

	    * A string that identifies the type of value expected for this
	      parameter. I.e. CHAR, INTEGER etc.

Usage:
	char *dest = "finder";
	GSContextRequirements_t *context;

	status = gs_inquire_context(dest, context)
		

Parameters:
	INPUT:
		char *receiver_name	- Receiver service name

	OUTPUT:
		GSContextRequirements_t *context  - Required parameters.


Status:
	On successful completion of this request, GS_SUCCESS is returned.
	If an error is encountered, one of the following codes may be
	returned:

	GSERR_MSGTYPE        - Unknown message type.
	GSERR_NOCONNECT      - Connect attempt failed.
	GSERR_EXECTYPE       - Unknown Executive Request.
	GSERR_SVC_UNAVAIL    - Unknown data service.
	GSERR_SVC_BADCONTEXT - The context args given to the
			     - receiver are bad or unsupported.

	GSERR_CM_NOTINIT     - The connection manager is not
			     - available, request is refused.


Author:
	Julian Carlisle 	 Tue Apr 30 01:30:47 PDT 1991
*****************************************************************************/


struct {
	int     type;
	char   *code;
}       GSArgTypes[] = {
	1, "INTEGER", 2, "CHAR", 3, "FLOAT", 4, "UINT", 0, NULL
};

int     gs_inquire_context(dest, context_req,count)
char   *dest;
GSContextRequirements_t ***context_req;
unsigned int *count;
{
	int     status = GS_SUCCESS;
	int     i, len;
	int     j, k;
	char   *cp = NULL;
	char  **tcp = (char **) 0;
	char   *pitem;
	GSHandle_t *h = (GSHandle_t *) 0;
	GSMessage_t msg, reply;
	GSContextRequirements_t **crp;

	*count = 0;
	h = gs_get_handle();
	if (h == (GSHandle_t *) 0) {
		gs_log("gs_inq_context: null handle from gs_get_handle\n");
		return (GSERR_CM_NOTINIT);
	}

#if 0
	if (h->debug) {
		gs_sem_dump(" (inq ctxt) locksem", h->cmslot->locksem);
		gs_sem_dump(" (inq ctxt) msgsem", h->cmslot->msgsem);
		gs_sem_dump(" (inq ctxt) cmsem", h->cmslot->cmsem);
	}
#endif

	msg.MsgType = Executive;
	msg.MsgBuf[0] = InqReceiverArgs;
	msg.MsgLen = 1;
	strcpy((char *) &msg.MsgBuf[1], dest);
	msg.MsgLen += strlen(dest);

	reply.MsgType = 0;
	reply.MsgLen = 0;
	reply.MsgBuf[0] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */
	status = gs_talk(h, h->cmslot, &msg, &reply);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_inq_context: talk status=%u - detaching slot\n", status);
		gs_detach_slot(h->cmslot);
		h->init = FALSE;
		*count = 0;
		return (status);
	}


 /*
  * handle the various possible replies from the Connection Manager
  */
	switch (reply.MsgType) {
	case Status:
		status = (int) reply.MsgBuf[0];
		if (h->debug)
			gs_log("gs_inq_context: Reply is Status %d\n", status);
		break;

	case Data:

		if (reply.MsgLen <= 1) {
			if (h->debug)
				gs_log("gs_inq_context: No entries returned.");
			status = GS_FAIL;
			break;
		}


		cp = (char *) &reply.MsgBuf[0];
		cp = strtok(cp, "|");
		for (i = 0; cp != NULL; i++, cp = strtok(NULL, "|")) {
			gs_tcp_append(&tcp, cp);
		}

	/* create and fill a vector of GSContextRequirement_t structs */
		crp = (GSContextRequirements_t **)
			malloc(sizeof(GSContextRequirements_t *) * i + 1);

		status = GS_SUCCESS;

		for (j = 0; j < i; j++) {
			crp[j] = (GSContextRequirements_t *)
				malloc(sizeof(GSContextRequirements_t) + 1);
			pitem = strtok(tcp[j], ",");
			gs_mk_string(pitem, &crp[j]->prompt);
			pitem = strtok(NULL, ",");
			if (pitem)
				gs_mk_string(pitem, &crp[j]->cmdline_tag);
			pitem = strtok(NULL, ",");
			if (pitem) {
				len = strlen(pitem);
				for (k = 0; GSArgTypes[k].code; k++) {
					if (strncmp(GSArgTypes[k].code, pitem, len) == 0) {
						crp[j]->type = GSArgTypes[k].type;
						break;
					}
				}
				if (crp[j]->type == 0)
					status = GSERR_SVC_BADCONTEXT;
			}

		}
		crp[j] = (GSContextRequirements_t *) 0;
		*context_req = crp;
		*count = j;
		break;

	default:
		if (h->debug)
			gs_log("gs_inq_context: Msg Type:%d  Len:%d .\n",
				reply.MsgType, reply.MsgLen);
		status = GSERR_MSGTYPE;

		*context_req = (GSContextRequirements_t **) 0;
		*count = 0;
		break;
	}


 /*
  * Free up the temporary connection context we have established.
  */
	if (gs_detach_slot(h->cmslot) == GS_SUCCESS)
		h->init = FALSE;
	else
		gs_log("gs_inquire_context: error detaching cmslot\n");

	return (status);
}
