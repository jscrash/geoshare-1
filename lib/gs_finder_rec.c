#include "gs.h"
#include <sys/ipc.h>
#include <sys/shm.h>

/*************************************************/
/*                                               */
/*      DATA RECEIVER SAMPLE IMPLEMENTATION      */
/*  This program implements a skeleton Finder    */
/*  data receiver                                */
/*                                               */
/* Julian Carlisle  Thu Apr 18 19:16:33 PDT 1991 */
/*************************************************/

#define RETRYS 4

int     main(argc, argv)     /* Data Receiver Main.  */
int     argc;
char  **argv;
{
	GSHandle_t *handle;
	GSTCB_t *ptcb;
	int     status = GS_SUCCESS;
	int     reqtype = 0;
	int     done = 0;
	int     trys = 0;
	char    text[256];


 /*
  * Call gs_connect.  This routine extracts the connection args from the
  * argument list and return a new argument list from which the
  * connection-arguments have been deleted. The handle returned by gs_connect
  * identifies the communication channel used to talk with the Connection
  * Manager and the Client application.
  */

	gs_print_args(argc,argv);
	handle = (GSHandle_t *) gs_connect((int *) &argc, (char ***) &argv);
	if (handle == (GSHandle_t *) 0) {
		gs_log("Failed to connect to Client.");
		exit(GS_FAIL);
	}
	while (1) {
		status = gs_receive(handle, &ptcb);
		if (status == GS_SUCCESS) {
			gs_log("Receive status=%d, tcb blocknum=%d",status,ptcb->BlockNum);
		}
		else if (status == GS_TERMINATE) {
			gs_log("rec:Terminating...receive returned %d status.", status);
			sleep(1);
			exit(0);
		}
		else {
			gs_log("rec:Unkown status from gs_receive: %lu",status);
			sleep(1);
			exit(0);
		}
	}
}
	/*NOTREACHED*/
#if 0

 /*
  * Receiver Main Loop - Read messages & process until terminate message
  * received.
  */
	do {
		status = GS_receive(handle, &tcb);

	/*
	 * If a receive fails, try again RETRYS times before aborting
	 */
		if (status != GS_SUCCESS) {
			Error_Handler(handle, "Error From gs_receive", status);
			trys++;
			if (trys > RETRYS) {
				sprintf(text, "Aborting after %d retrys.", trys);
				Error_Handler(handle, text, status);
				gs_term(handle);
				exit(GS_FAIL);
			}
			else {
				continue;	/* Try again to receive */
			}
		}

	/*
	 * Get here if receive was successful, reset the retry counter and
	 * process the message we have received.
	 */
		trys = 0;


	/*
	 * Process the message.  Determine if it is a Data message or an
	 * Executive request message.
	 */
		switch (tcb.data.type.symbolic) {

		/*
		 * Process this data message.
		 */
		case Data:
			store_data(&tcb.data);
			done = FALSE;
			break;

		/*
		 * An executive message has arrived.  Process the request and
		 * send back an acknowledge message.
		 */
		case Executive:
			Process_Executive_request(&tcb.data, &reqtype);
			if (reqtype == GS_STRUCT_RECEIVER_TERMINATE)
				done = TRUE;
			break;


		/*
		 * If message type is not recognized, log error to Connection
		 * Manager. For now this will be non-terminal even though it
		 * represents an unknown state that is difficult to recover
		 * from.
		 */
		default:
			Error_Handler(handle, "Invalid message type", tcb.data.type);
			break;
		}
	} while (!done);
	status = gs_term(handle);
	exit(status);
}
#endif


#if 0

/****************************************************************/
/* Error handler.  Send a status message back to the Connection        */
/* Manager.  Include the Error code & message string.                  */
/****************************************************************/
static void Error_Handler(handle, string, code)
GSHandle_t handle;
char   *string;
int     code;
{
	GSTCB_t tcb;
	GSMessage_t msg;
	int     result;
	char    text[256];

	sprintf(text, "%d:%s", code, string);
	strcpy(msg.MsgBuf, text);
	msg.MsgType = Executive;
	msg.MsgLen = strlen(text);

	strcpy(tcb.id, "TCB");
	tcb.BlockNum = 0;
	tcb.LastBlock = 0;
	tcb.data.structure_pointer = (GSVoidptr_t) &msg;

	if (handle != (GSHandle_t) 0) {
		gs_send(handle, &tcb, &result);
	}
	else {
	/* Write to an error log here */
	}
}
#endif
