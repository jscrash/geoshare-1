#include "gs.h"

/****************************************************************************
Module: GS_GET_HANDLE

Description:
	This routine returns a pointer to an allocated and initialized
	GSHandle_t structure.  The first time this routine is called, the
	pointer is allocated and the Connection Manager slot is initialized
	and attached.  

	On subsequent calls to this routine, the pointer to the already created
	handle is returned.  This routine may be called at any time, and as many
	times as desired.  The connection manager slot is initialized by using
	a key created from the path to the gstab file returned by gs_cf_name.

	If the connection manager slot shared memory segment/semaphores do not
	exist at the time of the first call to this routine or any other error is
	encountered, a NULL handle pointer is returned. 

	The caller must always verify the handle pointer is not null before using
	it.

Usage:
	GSHandle_t *handle;
	handle = gs_get_handle();
	if (handle == (GSHandle_t *)0) gs_log("Null handle error.");

Parameters:
	INPUT:
			NONE
	OUTPUT:
			NONE

Status:
	This routine returns a null handle pointer if any errors are encountered.

Author:
	Julian Carlisle 	 Tue Apr 30 01:39:57 PDT 1991
*****************************************************************************/

GSHandle_t *gs_get_handle()
{
	static GSHandle_t *h = (GSHandle_t *) 0;
	static int firsttime = TRUE;

	if (h != (GSHandle_t *) 0) {
		if (h->init != TRUE)
			firsttime = TRUE;
	}
	else {
		firsttime = TRUE;
	}

	if (firsttime == TRUE) {
		if (h == (GSHandle_t *) 0) {
			h = (GSHandle_t *) malloc(sizeof(GSHandle_t) + 4);
		}

		h->debug = gs_chk_debug();

		if (gs_home(h->homedir) == NULL) {
			if (h->debug)
				fprintf(stderr, "gs_get_handle:Can't get GSHOME directory\n");
			free(h);
			h = (GSHandle_t *) 0;
			return (h);
		}
		if (gs_cf_name(h->cfgfile) == NULL) {
			if (h->debug)
				fprintf(stderr, "gs_get_handle: Can't get config file name.\n");
			free(h);
			h = (GSHandle_t *) 0;
			return (h);
		}

		gs_logfile(h->logfile);

		h->pid = getpid();

		h->cmkey = (GSKey_t *) gs_ipc_key(h->cfgfile);
		if (h->cmkey == (GSKey_t *) 0) {
			if (h->debug) {
				fprintf(stderr,"gs_get_handle: Error generating shm key.\n");
			}
			free(h);
			h = (GSHandle_t *) 0;
			return (h);
		}

		if (gs_attach_slot(h->cmkey, &h->cmslot) != GS_SUCCESS) {
			if (h->debug) {
				fprintf(stderr,"gs_get_handle: Error attaching to cm slot.");
			}
			free(h);
			h = (GSHandle_t *) 0;
			return (h);
		}

		h->init = TRUE;
		firsttime = FALSE;
	}
	return (h);
}
