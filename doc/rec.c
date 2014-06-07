
/*************************************************/
/*                                               */
/*      DATA RECEIVER SAMPLE IMPLEMENTATION      */
/*                                               */
/*************************************************/

#include "geoscience.h"

 /*
  * STATUS CODES
  */
#define GS_SUCCESS                0        /* Successful Completion */
#define GS_FAIL                        1        /* Operation Failed */

#define GS_ERR_MSGTYPE                3        /* Unknown message type */
#define GS_ERR_NOCONNECT        5        /* Connect attempt failed */
#define GS_ERR_EXECTYPE                7        /* Unknown Executive Request */
#define GSERR_SVC_UNAVAIL         9        /* Unknown data service */
#define GSERR_SVC_BUSY                 11        /* Requested data receiver is single */
 /* threaded, and already in use */
#define GSERR_SVC_MISMATCH         13        /* The data type sent to the data */
 /* service is not supported.  */
#define GSERR_SVC_BADCONTEXT         15        /* The context args given to the */
 /* receiver are bad or unsupported. */
#define GSERR_CM_NOTINIT         17        /* The connection manager is not */
 /* available, request is refused.  */

#define GSMAXMESSAGE         1024
#define GSMAXKEY        64

/* Shared memory identifier key */
typedef struct shmkey {
    int     keylen;
    char    key[GSMAXKEY];
}       GS_shmkey_t;


 /*
  * Communications Handle Definition
  */
typedef unsigned char *GSHandle_t;


 /*
  * Message Definitions
  */
typedef enum {
    Data = 1,
    Executive = 2
} gs_msgtype_t;

typedef struct gs_message {
    gs_msgtype_t MsgType;
    unsigned int MsgLen;
    unsigned char MsgBuf[GSMAXMESSAGE];
}       GS_MESSAGE;

typedef struct tcb_t {
    char    id[4];
    int     BlockNum;
    int     LastBlock;
    Structure_t data;
}       TCB;
 /* Send Error string to Connection Manager if handle         */
 /* is valid. Otherwise just write the string to the          */
 /* local error log in   the GSHOME directory.                  */
#ifndef sun
void    Error_Handler(GSHandle_t h, char *str, int code);
#else
void    Error_Handler();
#endif

 /* Process an Executive request message.  Return the          */
 /* type of request processed in the request_type arg.         */
#ifndef sun
void    Process_executive(Structure_t data, DataStructureType_t * request_type);
#else
void    Process_executive();
#endif
 /* Local defines. */
#define TRUE 1
#define FALSE 2
#define RETRYS 4


int     main(argc, argv)     /* Data Receiver Main.  */
int     argc;
char  **argv;
{
    GSHandle_t handle;
    TCB     tcb;
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

    handle = (GSHandle_t) gs_connect((int *) &argc, (char ***) &argv);
    if (handle == (GSHandle_t) 0) {
        Error_Handler(handle, "Failed to connect to Client.", GS_FAIL);
        exit(GS_FAIL);
    }



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
                continue;    /* Try again to receive */
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
         * An executive message has arrived.  Process the request and send
         * back an acknowledge message.
         */
        case Executive:
            Process_Executive_request(&tcb.data, &reqtype);
            if (reqtype == GS_STRUCT_RCVR_TERMINATE)
                done = TRUE;
            break;


        /*
         * If message type is not recognized, log error to Connection
         * Manager. For now this will be non-terminal even though it
         * represents an unknown state that is difficult to recover from.
         */
        default:
            Error_Handler(handle, "Invalid message type", tcb.data.type);
            break;
        }
    } while (!done);
    status = gs_term(handle);
    exit(status);
}




/****************************************************************/
/* Error handler.  Send a status message back to the Connection        */
/* Manager.  Include the Error code & message string.                  */
/****************************************************************/
static void Error_Handler(handle, string, code)
GSHandle_t handle;
char   *string;
int     code;
{
    TCB     tcb;
    GS_MESSAGE msg;
    int     result;
    char    text[256];

    sprintf(text, "%d:%s", code, string);
    strcpy(msg.MsgBuf, text);
    msg.MsgType = Executive;
    msg.MsgLen = strlen(text);

    strcpy(tcb.id, "TCB");
    tcb.BlockNum = 0;
    tcb.LastBlock = 0;
    tcb.data.structure_pointer = (Voidptr_t) & msg;

    if (handle != (GSHandle_t) 0) {
        gs_send(handle, &tcb, &result);
    }
    else {
    /* Write to an error log here */
    }
}
