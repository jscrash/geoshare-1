#ifndef _GS_ERR_H
#define _GS_ERR_H 1
/*************************************************/
/*                                               */
/*      GeoShare library routine error codes     */
/*                                               */
/*************************************************/

#define GSERR_MSGTYPE        3	/* Unknown message type */
#define GSERR_NOCONNECT      5	/* Connect attempt failed */
#define GSERR_EXECTYPE       7	/* Unknown Executive Request */
#define GSERR_SVC_UNAVAIL    9	/* Unknown data service */
#define GSERR_SVC_BUSY       11	/* Requested data receiver is single */
 /* threaded, and already in use */

#define GSERR_SVC_MISMATCH   13	/* The data type sent to the data */
 /* service is not supported.  */

#define GSERR_SVC_BADCONTEXT 15	/* The context args given to the */
 /* receiver are bad or unsupported. */

#define GSERR_CM_NOTINIT     17	/* The connection manager is not */
 /* available, request is refused.  */

#define GSERR_OPT_UNAVAIL	 19	/* Config file option not found for entry */

#ifndef SUCCESS
#define SUCCESS              ((long)0)
#endif
#ifndef FAIL
#define FAIL                 ((long)-1)
#endif

#define GS_SUCCESS        SUCCESS	/* Successful Completion */
#define GS_FAIL           FAIL	/* Operation Failed */
#endif
