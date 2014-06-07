#include "gs.h"
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/****************************************************************************
Module: GS_INITIALIZE

Description:
	Initialize a connection to the  named  receiver  program.
	This will send a protocol request to the connection manager
	running on the current host and have it start the named program.
	The name of the program may be an alias that requires translation
	by the connection manager process.

	Upon successful initialization of the requested connection, a
	communications handle is returned.  This handle is used in
	subsequent calls to GeoShare library routines to identify the
	connection channel.


Usage:
	handle  = gs_initialize(destination, context)
	
Parameters:
	INPUT:
           char   *destination		   - Name of data receiver pgm to start
           GSContextInformation_t context  - Arguments for receiver command line

	OUTPUT:
		None

Status:
	Upon successful completion, this routine returns a valid GSHandle_t
	pointer.

	If an error occurrs during initialization, one of the following
	error codes will be passed back in the status field of the handle.

	GSERR_SVC_UNAVAIL    - Unknown data service.
	GSERR_SVC_BUSY       - Requested data receiver is single
			     - threaded, and already in use.

	GSERR_SVC_BADCONTEXT - The context args given to the
			     - receiver are bad or unsupported.

	GSERR_CM_NOTINIT     - The connection manager is not
			     - available, request is refused.


Author:
	Julian Carlisle 	 Fri Apr 19 12:16:49 PDT 1991
*****************************************************************************/
static int gsz_create_slot();

GSHandle_t *gs_initialize(dest, context,st)
char   *dest;
char   *context;
int *st;
{

	int     status = GS_SUCCESS;
	char    text[256],*cp;
	GSHandle_t *h = (GSHandle_t *) 0;
	GSMessage_t msg, reply;
	int     len,i;
	char    filename[256];
	int     fd,*pint;


	h = gs_get_handle();
	if (h == (GSHandle_t *) 0) {
		gs_log( "gs_initialize: null handle from gs_get_handle\n");
		*st = GS_FAIL;
		return ((GSHandle_t *) 0);
	}

 /* Create a local slot for communication with receiver. */
	sprintf(text, "%s/gsappkey_XXXXXX", h->homedir);
	strcpy(filename, mktemp(text));
	if (strlen(filename) <= 1) {
		gs_log("gs_initialize: cannot create slotfile name.");
		*st = GS_FAIL;
		return ((GSHandle_t *) 0);
	}
	fd = open(filename, O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		gs_log("gs_initialize: cannot create slotfile");
		*st = GS_FAIL;
		return ((GSHandle_t *) 0);
	}
	close(fd);

	h->key = (GSKey_t *) gs_ipc_key(filename);
	status = gsz_create_slot(h->key, &h->slot);
	if (status != GS_SUCCESS) {
		gs_log("gs_initialize: Error creating app slot.");
		*st = GS_FAIL;
		return ((GSHandle_t *) 0);
	}

 /* preset the message semaphore to > 0.  (available) */
	gs_semV(h->slot->msgsem);
	errno = 0;

	cp = context;
	while(*cp == ' ')
		cp++;
	
	msg.MsgLen = 0;
	msg.MsgType = Executive;
	msg.MsgBuf[0] = StartReceiver;
	msg.MsgLen++;
	sprintf(text, "%s:-slot %s %s", dest, filename, cp);
	len = strlen(text);
	len++;		     /* null counts */
	memcpy(&msg.MsgBuf[1], text, len);
	msg.MsgLen += len;
	msg.pid = getpid();
	msg.shmid = h->slot->id;
	msg.key = h->key->key;
	reply.MsgType = 0;
	reply.MsgLen = 0;
	for(i=0; i<sizeof(int);i++)
		reply.MsgBuf[i] = '\0';

 /*
  * Talk to the connection manager, wait for reply
  */
	status = gs_talk(h, h->cmslot, &msg, &reply);
	if (status != GS_SUCCESS) {
		if (h->debug)
			gs_log("gs_initialize: talk status=%d - Deleting slot\n", status);

		if (gsz_delete_slot(h->slot) == GS_SUCCESS)
			h->init = FALSE;
		*st = status;
		return ((GSHandle_t *) 0);
	}
	if (reply.MsgType == Status) {
		pint = (int *)&reply.MsgBuf[0];
		if (*pint > 1) {
			h->pid = *pint;
			if (h->debug) 
				gs_log("gs_initialize: receiver process is %u",h->pid);
		}
		else if(*pint == GS_FAIL) {
			gs_log("gs_initialize: Deleting slot. Reply status msg=GS_FAIL");
			if (gsz_delete_slot(h->slot) == GS_SUCCESS)
				h->slot = (GSSlot_t *)0;
				*st = GSERR_NOCONNECT;
			return ((GSHandle_t *) 0);
		}
		else {
			gs_log("gs_initialize: Warning: Child PID out of range: %d",*pint);
		}
	}
	else {
		gs_log("gs_initialize: CM returned %u message - Deleting slot.",
				reply.MsgType);
				
		if (gsz_delete_slot(h->slot) == GS_SUCCESS)
			h->init = FALSE;
		*st = GSERR_MSGTYPE;
		return ((GSHandle_t *) 0);
	}
	return (h);
}

static int gsz_create_slot(key, pslot)
GSKey_t *key;
GSSlot_t **pslot;
{
	int     id = 0;

	errno = 0;

	if (key == (GSKey_t *) 0) {
		gs_log("gsz_create_slot: Null key.");
		return (GS_FAIL);
	}

 /* Create shared memory segment, return its id.	 */
	id = shmget(key->key, sizeof(GSSlot_t), 0666 | IPC_CREAT);
	if (id < 0) {
		gs_log("gsz_create_slot:Cannot alloc shared memory for slot.");
		return (GS_FAIL);
	}

 /* Attach the shared memory, let the system 	 */
 /* choose the address at which the memory is 	 */
 /* attached.  This is more portable.		 */
	*pslot = (GSSlot_t *) shmat(id, (char *) 0, 0);
	if (*pslot == (GSSlot_t *) -1) {
		gs_log("gsz_create_slot:Cannot attach shared memory.");
		return (GSERR_CM_NOTINIT);
	}

(*pslot)->id = id;

 /* Create the slot lock semaphore. 	 */
	(*pslot)->lockkey = ftok(key->str,'0');
	if ((*pslot)->lockkey > 0)
		(*pslot)->locksem = gs_semtran((*pslot)->lockkey);
	if ((*pslot)->locksem < 0 || (*pslot)->lockkey == -1) {
	/* remove the shared memory segment  	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gsz_create_slot:Cannot remove shared mem.");
		}
		gs_log("gsz_create_slot:Cannot create lock semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->msgkey = ftok(key->str,'1');
	if ((*pslot)->msgkey > 0)
		(*pslot)->msgsem = gs_semtran((*pslot)->msgkey);
	if ((*pslot)->msgsem < 0 || (*pslot)->msgkey == -1) {
	gs_log("gsz_create_slot:error creating msgsem. Deleting locksem");
	/* remove the semaphore created above. 	 */
		gs_semrm((*pslot)->locksem);

	/* remove the shared memory segment	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gsz_create_slot:Cannot remove shared mem.");
		}

		gs_log("gsz_create_slot:Cannot create msg semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->cmkey = ftok(key->str,'2');
	if ((*pslot)->cmkey > 0)
		(*pslot)->cmsem = gs_semtran((*pslot)->cmkey);
	if ((*pslot)->cmsem < 0 || (*pslot)->cmkey == -1) {
	gs_log("gsz_create_slot:error creating cmsem. Deleting locksem&msgsem");
	/* remove the semaphore created above. 	 */
		gs_semrm((*pslot)->locksem);
		gs_semrm((*pslot)->msgsem);

	/* remove the shared memory segment	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gsz_create_slot:Cannot remove shared mem.");
		}

		gs_log("gsz_create_slot:Cannot create cm semaphore");
		return (GS_FAIL);
	}

	return (GS_SUCCESS);
}
