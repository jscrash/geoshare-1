 /*
  * GeoShare system header file.
  * 
  * This file contains all of the definitions required by GeoShare interface
  * library routines.
  * 
  */

#ifndef GS_H
#define GS_H 1

#ifndef _GEOSCIENCE_H
#include "geoscience.h"
#endif


/*************************************************/
/*                                               */
/*      GEOSHARE LIBRARY HEADER                  */
/*                                               */
/*************************************************/


 /*
  * STATUS CODES
  */
#define GS_SUCCESS              0        /* Successful Completion */
#define GS_FAIL                 1        /* Operation Failed */

#define GS_ERR_MSGTYPE          3        /* Unknown message type */
#define GS_ERR_NOCONNECT        5        /* Connect attempt failed */
#define GS_ERR_EXECTYPE         7        /* Unknown Executive Request */
#define GSERR_SVC_UNAVAIL       9        /* Unknown data service */
#define GSERR_SVC_BUSY          11       /* Requested data receiver is single */
                                         /* threaded, and already in use */
#define GSERR_SVC_MISMATCH      13       /* The data type sent to the data */
                                         /* service is not supported.  */
#define GSERR_SVC_BADCONTEXT    15       /* The context args given to the */
                                         /* receiver are bad or unsupported. */
#define GSERR_CM_NOTINIT        17       /* The connection manager is not */
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
 /* local error log in the GSHOME directory.                  */
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

#endif
