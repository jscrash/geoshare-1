#include "gs.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <math.h>

/****************************************************************************
Module:	GS_CONNECT

Description:
	This routine is called by data receiver programs in order
	to establish a connection with the GeoShare connection Manager
	process.  This routine will return a communication handle upon
	successful completion.  The handle returned is used in calls such as
	gs_send to identify the channel that is connected to the sending
	application.

Usage:
	handle = gs_connect(&argc,&argv);
	
Parameters:
	INPUT:
	    int *argc		- Address of the argc from callers main.
	    char ***argv	- Address of argv array from caller.

	OUTPUT:
	    GSHandle_t  handle	- handle for subsequent communication.

Status:
	This routine will return a NULL ('\0') handle if an error occurrs
	while connecting to the sender or connection manager processes.


Author:
	Julian Carlisle 	 Sat Apr 20 11:28:14 PDT 1991
*****************************************************************************/

GSHandle_t *gs_connect(ac, av)
int    *ac;
char ***av;
{
	int     status = GS_SUCCESS;
	char  **newargv;
	int     argc, len;
	char  **argv, *cp;
	int     i, i2;
	char    slotkey[64];
	GSHandle_t *h;

	argc = (int) *ac;
	argv = (char **) *av;
	newargv = (char **) malloc(sizeof(char *) * argc + 1);
	for (i = 0, i2 = 0; argv[i] != (char *) 0; i++) {
		cp = argv[i];
		if (*cp == '-') {
			if (strcmp(cp + 1, "slot") == 0) {
				if (argv[i + 1] != (char *) 0) {
					strcpy(slotkey, argv[i + 1]);
					i++;
					continue;
				}
			}
		}
		len = strlen(cp);
		newargv[i2] = (char *) malloc(sizeof(char) * len + 1);
		strcpy(newargv[i2], argv[i]);
		i2++;
	}
	*ac = i2;
	*av = (char **) newargv;

	signal(SIGALRM,SIG_IGN);
	alarm(0);
	signal(SIGHUP,SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	
	gs_set_debug(TRUE);

	umask(2);
	h = (GSHandle_t *) malloc(sizeof(GSHandle_t) + 4);
	if (h == (GSHandle_t *) 0) {
		gs_log("rec:Out of memory. cannot allocate handle");
		return(h);
	}
	h->debug = gs_chk_debug();

	if ((cp = gs_home(0)) != NULL)
		strcpy(h->homedir, cp);
	else {
		free(h);
		h = (GSHandle_t *) 0;
		gs_log("rec:Can't get GSHOME directory\n");
		return (h);
	}
	if ((cp = gs_cf_name(0)) != NULL)
		strcpy(h->cfgfile, cp);
	else
		h->cfgfile[0] = '\0';

	if ((cp = gs_logfile(0)) != NULL)
		strcpy(h->logfile, cp);

	h->pid = getpid();
	h->init = TRUE;
	h->key = (GSKey_t *) gs_ipc_key(slotkey);

	 /* connect to ap slot. */
	if ((status = gsz_attach_slot(h->key, &h->slot)) != GS_SUCCESS) {
		gs_log("rec:Error attaching  to app slot, keyfile=%s",slotkey);
		free(h);
		h = (GSHandle_t *)0;
	}
	return (h);
}



int     gsz_attach_slot(key, pslot)
GSKey_t *key;
GSSlot_t **pslot;
{
	int     id = 0;

	errno = 0;

	if (key == (GSKey_t *) 0) {
		gs_log("gsz_attach_slot: Null key.");
		return (GS_FAIL);
	}

 /* Create shared memory segment, return its id.	 */
	id = shmget(key->key, sizeof(GSSlot_t), 0);
	if (id < 0) {
		gs_log("gsz_attach_slot:Cannot attach shared memory for slot.");
		return (GS_FAIL);
	}


 /* Attach the shared memory, let the system 	 */
 /* choose the address at which the memory is 	 */
 /* attached.  This is more portable.		 */
	*pslot = (GSSlot_t *) shmat(id, (char *) 0, 0);
	if (*pslot == (GSSlot_t *) -1) {
		gs_log("gsz_attach_slot:Cannot attach shared memory.");
		return (GS_FAIL);
	}

 /* Create the slot lock semaphore. 	 */
	(*pslot)->lockkey = ftok(key->str,'0');
	if ((*pslot)->lockkey > 0)
		(*pslot)->locksem = gs_semtran((*pslot)->lockkey);
	if ((*pslot)->locksem < 0 || (*pslot)->lockkey == -1) {
		gs_log("gsz_attach_slot:Cannot open lock semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->msgkey = ftok(key->str,'1');
	if ((*pslot)->msgkey > 0)
		(*pslot)->msgsem = gs_semtran((*pslot)->msgkey);
	if ((*pslot)->msgsem < 0 || (*pslot)->msgkey == -1) {

	/* remove the semaphore open above. 	 */
		gs_semrm((*pslot)->locksem);

		gs_log("gsz_attach_slot:Cannot attach msg semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->cmkey = ftok(key->str,'2');
	if ((*pslot)->cmkey > 0)
		(*pslot)->cmsem = gs_semtran((*pslot)->cmkey);
	if ((*pslot)->cmsem < 0 || (*pslot)->cmkey == -1) {

	/* remove the semaphore opened above. 	 */
		gs_semrm((*pslot)->locksem);
		gs_semrm((*pslot)->msgsem);

		gs_log("gsz_attach_slot:Cannot open cm semaphore");
		return (GS_FAIL);
	}
	(*pslot)->id = id;

	return (GS_SUCCESS);
}


