#include "gs.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/****************************************************************************
    GeoShare Subroutine Library -  Shared Memory Tools.

Module: GS_CR_SLOT

Description:
	Create a communications slot.  A slot is composed of two semaphores
	and a buffer.  This routine allocates a region of shared memory
	large enough to hold a slot.  Two semaphores are created in the slot.

	A key is given in the parameter list that is used to identify the
	shared memory segment, and semaphores created.

Usage:
	GSKey_t *key;
	GSSlot_t slot;

	status = gs_create_slot.c(key,&slot);

	
Parameters:
	INPUT:
		GSKey_t *key		- Memory identifier key pointer.

	OUTPUT:
		char **pslot		- Pointer to slot allocated.

Status:

	This routine returns GS_SUCCESS or GS_FAIL;

Author:
	Julian Carlisle 	 Wed Apr 24 03:38:02 PDT 1991
*****************************************************************************/

int     gs_create_slot(key, pslot)
GSKey_t *key;
GSSlot_t **pslot;
{
	int     id = 0;

	errno = 0;

	if (key == (GSKey_t *) 0) {
		gs_log("gs_create_slot: Null key.");
		return (GS_FAIL);
	}

 /* Create shared memory segment, return its id.	 */
	id = shmget(key->key, sizeof(GSSlot_t), 0666 | IPC_CREAT);
	if (id < 0) {
		gs_log("gs_create_slot:Cannot alloc shared memory for slot.");
		return (GS_FAIL);
	}

 /* Attach the shared memory, let the system 	 */
 /* choose the address at which the memory is 	 */
 /* attached.  This is more portable.		 */
	*pslot = (GSSlot_t *) shmat(id, (char *) 0, 0);
	if (*pslot == (GSSlot_t *) -1) {
		gs_log("gs_create_slot:Cannot attach shared memory.");
		return (GS_FAIL);
	}
	(*pslot)->id = id;

 /* Create the slot lock semaphore. 	 */
	(*pslot)->lockkey = ftok(key->str,'0');
	if ((*pslot)->lockkey > 0)
		(*pslot)->locksem = gs_sem_create((*pslot)->lockkey, 1);
	if ((*pslot)->locksem < 0 || (*pslot)->lockkey == -1) {
	/* remove the shared memory segment  	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gs_create_slot:Cannot remove shared mem.");
		}
		gs_log("gs_create_slot:Cannot create lock semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->msgkey = ftok(key->str,'1');
	if ((*pslot)->msgkey > 0)
		(*pslot)->msgsem = gs_sem_create((*pslot)->msgkey, 1);
	if ((*pslot)->msgsem < 0 || (*pslot)->msgkey == -1) {

	/* remove the semaphore created above. 	 */
		gs_sem_close((*pslot)->locksem);

	/* remove the shared memory segment	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gs_create_slot:Cannot remove shared mem.");
		}

		gs_log("gs_create_slot:Cannot create msg semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->cmkey = ftok(key->str,'2');
	if ((*pslot)->cmkey > 0)
		(*pslot)->cmsem = gs_sem_create((*pslot)->cmkey, 0);
	if ((*pslot)->cmsem < 0 || (*pslot)->cmkey == -1) {
		gs_log("gs_create_slot: Error creating cmsem. closing locksem,msgsem");
	/* remove the semaphore created above. 	 */
		gs_sem_close((*pslot)->locksem);
		gs_sem_close((*pslot)->msgsem);

	/* remove the shared memory segment	 */
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0) {
			gs_log("gs_create_slot:Cannot remove shared mem.");
		}

		gs_log("gs_create_slot:Cannot create cm semaphore");
		return (GS_FAIL);
	}
	(*pslot)->id = id;

	return (GS_SUCCESS);
}




/****************************************************************************
    GeoShare Subroutine Library -  Shared Memory Tools.

Module: GS_AT_SLOT

Description:
	Attach to a communications slot.  A slot is composed of two semaphores
	and a buffer.  This routine attaches to a region of shared memory
	formatted as a slot that has been previously created.  Two semaphores
	are also attached in the slot.

	A key is given in the parameter list that is used to identify the
	shared memory segment, and semaphores to attach to.

Usage:
	GSKey_t *key;
	GSSlot_t slot;

	status = gs_attach_slot(key,&slot);

	
Parameters:
	INPUT:
		GSKey_t *key		- Memory identifier key pointer.

	OUTPUT:
		char **pslot		- Pointer to slot attached.

Status:

	This routine returns GS_SUCCESS or GS_FAIL;

Author:
	Julian Carlisle 	 Wed Apr 24 03:38:02 PDT 1991
*****************************************************************************/

int     gs_attach_slot(key, pslot)
GSKey_t *key;
GSSlot_t **pslot;
{
	int     id = 0;

	errno = 0;

	if (key == (GSKey_t *) 0) {
		gs_log("gs_attach_slot: Null key.");
		return (GS_FAIL);
	}

 /* Create shared memory segment, return its id.	 */
	id = shmget(key->key, sizeof(GSSlot_t), 0);
	if (id < 0) {
		gs_log("gs_attach_slot:Cannot attach shared memory for slot.");
		return (GS_FAIL);
	}


 /* Attach the shared memory, let the system 	 */
 /* choose the address at which the memory is 	 */
 /* attached.  This is more portable.		 */
	*pslot = (GSSlot_t *) shmat(id, (char *) 0, 0);
	if (*pslot == (GSSlot_t *) -1) {
		gs_log("gs_attach_slot:Cannot attach shared memory.");
		return (GS_FAIL);
	}

 /* Create the slot lock semaphore. 	 */
	(*pslot)->lockkey = ftok(key->str,'0');
	if ((*pslot)->lockkey > 0)
		(*pslot)->locksem = gs_sem_open((*pslot)->lockkey);
	if ((*pslot)->locksem < 0 || (*pslot)->lockkey == -1) {
		gs_log("gs_attach_slot:Cannot open lock semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->msgkey = ftok(key->str,'1');
	if ((*pslot)->msgkey > 0)
		(*pslot)->msgsem = gs_sem_open((*pslot)->msgkey);
	if ((*pslot)->msgsem < 0 || (*pslot)->msgkey == -1) {

	/* remove the semaphore open above. 	 */
		gs_sem_close((*pslot)->locksem);

		gs_log("gs_attach_slot:Cannot attach msg semaphore");
		return (GS_FAIL);
	}

 /* Create the slot message semaphore. 	 */
	(*pslot)->cmkey = ftok(key->str,'2');
	if ((*pslot)->cmkey > 0)
		(*pslot)->cmsem = gs_sem_open((*pslot)->cmkey);
	if ((*pslot)->cmsem < 0 || (*pslot)->cmkey == -1) {

	/* remove the semaphore opened above. 	 */
		gs_sem_close((*pslot)->locksem);
		gs_sem_close((*pslot)->msgsem);

		gs_log("gs_attach_slot:Cannot open cm semaphore");
		return (GS_FAIL);
	}
	(*pslot)->id = id;

	return (GS_SUCCESS);
}





/****************************************************************************
    GeoShare Subroutine Library -  Shared Memory Tools.

Module: GS_DT_SLOT

Description:
	Detach a communications slot.  A slot is composed of two semaphores
	and a buffer.  This routine detaches the shared memory associated
	with a slot after closing the semaphores in the slot.


Usage:
	GSSlot_t *pslot;

	status = gs_detach_slot(pslot);

	
Parameters:
	INPUT:
		GSSlot_t *pslot		- Address of slot to detach.
	OUTPUT:
		None.

Status:

	This routine will return GS_SUCCESS or GS_FAIL.

Author:
	Julian Carlisle 	 Wed Apr 24 03:49:06 PDT 1991
*****************************************************************************/


int     gs_detach_slot(pslot)
GSSlot_t *pslot;
{
	errno = 0;

	if (pslot == (GSSlot_t *) 0) {
		gs_log("gs_detach_slot: Null slot pointer.");
		return (GS_FAIL);
	}

	gs_sem_close(pslot->locksem);
	gs_sem_close(pslot->msgsem);
	gs_sem_close(pslot->cmsem);

	if (gs_shm_free((char *)pslot) < 0) {
		gs_log("gs_detach_slot:Cannot detach shared memory %d", pslot);
		return (GS_FAIL);
	}

	return (GS_SUCCESS);
}




/****************************************************************************
    GeoShare Subroutine Library -  Shared Memory Tools.

Module: GS_DELETE_SLOT

Description:
	Delete a communications slot.  This routine detaches and deletes the
	shared memory associated with a slot after closing its semaphores.


Usage:
	GSSlot_t *pslot;

	status = gs_delete_slot(pslot);

	
Parameters:
	INPUT:
		GSSlot_t *pslot		- Address of slot to delete.

	OUTPUT:
		None.

Status:
	This routine will return GS_SUCCESS or GS_FAIL.

Author:
	Julian Carlisle 	 Fri May  3 16:12:22 PDT 1991
*****************************************************************************/

int     gs_delete_slot(pslot)
GSSlot_t *pslot;
{
	int     status = GS_SUCCESS;
	int     id = 0;

	errno = 0;
	if (pslot == (GSSlot_t *) 0) {
		gs_log("gs_delete_slot: Null slot pointer.");
		return (GS_FAIL);
	}

	gs_sem_rm(pslot->locksem);
	gs_sem_rm(pslot->msgsem);
	gs_sem_rm(pslot->cmsem);

	id = pslot->id;


	if (gs_shm_free((char *)pslot) < 0) {
		gs_log("gs_delete_slot:Cannot detach shared memory %d", pslot);
		status = GS_FAIL;
	}
	else {
		if (shmctl(id, IPC_RMID, (struct shmid_ds *) 0) < 0)
			gs_log("gs_delete_slot: Error deleting slot memory");
		else
			status = SUCCESS;
	}
	return (status);
}
