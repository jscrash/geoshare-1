#include "gs.h"
#include        <sys/ipc.h>
#include        <sys/sem.h>

#if 0
extern int semget();
extern int semctl();
extern int semop();

#endif
 /*
 * Provlde an simpler and easier to understand interface to the System V
 * semaphore system calls. There are 7 routines available to the user:
 *      id = gs_sem_create(key, initval); # create with initial value or open
 *      id = gs_sem_open(key)i              # open (must already exist)
 *      gs_sem_wait(id);                    # wait = P = down by 1
 *      gs_sem_signal(id);                  # signal = V = up by 1
 *      gs_sem_op(id, amount);              # wait if (amount < O)
 *                                       # signal if (amount > O)
 *      gs_sem_close(id);                   # close
 *      gs_sem_rm(id);                      # remove (delete)
 * We create and use a 3-member set for the requested semaphore.
 * The first member, [0], is the actual semaphore value, and the second
 * member, [1], is a counter used to know when all processes have finished
 * with the semaphore. The counter is initialized to a large number,
 * decremented on every create or open and incremented on every close.
 * This way we can use the "adjust" feature provided by System V so that
 * any process that exit's without calling gs_sem_close() is accounted
 * for. It doesn't help us if the last process does this (as we have
 * no way of getting control to remove the semaphore) but it will
 * work if any process other than the last does an exit (intentional
 * or unintentional).
 * The third member, [2], of the semaphore set is used as a lock variable
 * to avoid any race conditions in the gs_sem_create() and gs_sem_close()
 * functions.
 */


#define BIGCOUNT 10000	     /* initial value of process counter */

 /*
 * Define the semaphore operation arrays for the semop() calls.
 */

static struct sembuf op_lock[2] =
{
	2, 0, 0,	     /* wait for [2] (lock) to equal O */
	2, 1, SEM_UNDO	     /* then increment [2] to 1 - this locks it */

 /*
  * UNDO to release the lock if processes exits before explicitly unlocking
  */
};
static struct sembuf op_endcreate[2] =
{
	1, -1, SEM_UNDO,     /* decrement [1] (proc counter) with undo on exit */
 /* UNDO to adjust proc counter if process exits */
 /* before explicitly calling sem_close() */

	2, -1, SEM_UNDO	     /* then decrement [2] (lock) back to O */
};

static struct sembuf op_open[1] =
{
	1, -1, SEM_UNDO	     /* decrement [1] (proc counter) with undo on  exit */
};

static struct sembuf op_close[3] =
{
	2, 0, 0,	     /* wait for [2] (lock) to equal O */
	2, 1, SEM_UNDO,	     /* then increment [2] to 1 - this locks it */
	1, 1, SEM_UNDO	     /* then increment [1] (proc counter) */
};

static struct sembuf op_unlock[1] =
{
	2, -1, SEM_UNDO	     /* decrement [2] (lock) back to O */
};

static struct sembuf op_op[1] =
{
	0, 99, 0	     /* decrement or increment [0] with undo on exit */
};
					 /* 99 is set to the actual amount to add or subtract */



 /*
  * Create a semaphore with a specified initial value. If the semaphore
  * already exists, we don't initialize it (of course). We return the
  * semaphore ID if all OK, else GS_FAIL.
  */
int     gs_sem_create(key, initval)
key_t   key;
int     initval;	     /* used if we create the semaphore */
{
	register int id, semval;
	union semun {
		int     val;
		struct semid_ds *buf;
		ushort *array;
	}       semctl_arg;
#ifdef DEBUG
	gs_log("calling gs_sem_create");
#endif

	if (key == IPC_PRIVATE)
		return (GS_FAIL); /* not intended for private semaphores */

	else if (key == (key_t) - 1)
		return (GS_FAIL); /* probably an ftok() error by caller */

again:
#ifdef DEBUG
	gs_log("gs_sem_create: Calling semget\n");
#endif
	if ((id = semget(key, 3, 0666 | IPC_CREAT)) < 0)
		return id;   /* permission problem or tables full */

 /*
  * When the semaphore is created, we know that the value of all 3 members is
  * 0. Get a lock on the semaphore by waiting for [2] to equal 0, then
  * increment it. There is a race condition here. There is a possibility that
  * between the semget() above and the semop() below, another process can
  * call our gs_sem_close() function which can remove the semaphore if that
  * process is the last one using it. Therefore, we handle the error
  * condition of an invalid semaphore ID specially below, and if it does
  * happen, we just go back and create it again.
  */

#ifdef DEBUG
	gs_log("gs_sem_create: Locking semaphore %d\n", id);
#endif
	if (semop(id, &op_lock[0], 2) < 0) {
		if (errno == EINVAL)
			goto again;
		gs_log("gs_sem_create: can't lock");
	}

 /*
  * Get the value of the process counter. If it equals 0, then no one has
  * initialized the semaphore yet.
  */

	if ((semval = semctl(id, 1, GETVAL, 0)) < 0)
		gs_log("gs_sem_create: can't GETVAL");

#ifdef DEBUG
	gs_log("gs_sem_create: process counter = %d\n", semval);
#endif
	if (semval == 0) {

	/*
	 * We could initialize by doing a SETALL, but that would clear the
	 * adjust value that we set when we locked the semaphore above.
	 * Instead, we'll do 2 system calls to initialize [0] and [1].
	 */

		semctl_arg.val = initval;
		if (semctl(id, 0, SETVAL, semctl_arg) < 0)
			gs_log("gs_sem_create: can SETVAL[0]");

		semctl_arg.val = BIGCOUNT;
		if (semctl(id, 1, SETVAL, semctl_arg) < 0)
			gs_log("gs_sem_create: can SETVAL[1]");
	}

 /*
  * Decrement the process counter and then release the lock.
  */

	if (semop(id, &op_endcreate[0], 2) < 0)
		gs_log("gs_sem_create: can't end create");

	return (id);
}



 /*
  * Open a semaphore that must already exist. This function should be used,
  * instead of gs_sem_create(), if the caller knows that the semaphore must
  * already exist. For example a client from a client-server pair would use
  * this, if its the server's responsibility to create the semaphore. We
  * return the semaphore ID if all OK, else GS_FAIL.
  */

int     gs_sem_open(key)
key_t   key;
{
	register int id;
#ifdef DEBUG
	gs_log("calling gs_sem_open");
#endif

	if (key == IPC_PRIVATE)
		return (GS_FAIL); /* not intended for private semaphores */

	else if (key == (key_t) - 1)
		return (GS_FAIL); /* probably an ftok() error by caller */

	if ((id = semget(key, 3, 0)) < 0)
		return (GS_FAIL); /* doesn't exist, or tables full */

 /*
  * Decrement the process counter. We don't need a lock to do this.
  */

	if (semop(id, &op_open[0], 1) < 0)
		gs_log("gs_sem_open: can't open, key=%u", key);

	return (id);
}



 /*
  * Remove a semaphore. This call is intended to be called by a server, for
  * example, when it is being shut down, as we do an IPC RMID on the
  * semaphore, regardless whether other processes may be using it or not.
  * Most other processes should use gs_sem_close() below.
  */

int     gs_sem_rm(id)
int     id;
{
	return (semctl(id, 0, IPC_RMID, 0));
}




 /*
  * Close a semaphore. Unlike the remove function above, this function is for
  * a process to call before it exits, when it is done with the semaphore. We
  * "decrement" the counter of processes using the semaphore, and if this was
  * the last one, we can remove the semaphore.
  */

int     gs_sem_close(id)
int     id;
{
	register int semval;
	int     status = GS_SUCCESS;
#ifdef DEBUG
	gs_log("calling gs_sem_close");
#endif

 /*
  * The following semop() first gets a lock on the semaphore, then increments
  * [l] - the process counter.
  */
	if (semop(id, &op_close[0], 3) < 0)
		gs_log("gs_sem_close:can't semop");

 /*
  * Now that we have a lock, read the value of the process counter to see if
  * this is the last reference to the semaphore. There is a race condition
  * here - see the comments in gs_sem_create().
  */

	if ((semval = semctl(id, 1, GETVAL, 0)) < 0)
		gs_log("gs_sem_close: can't GETVAL");

#ifdef DEBUG
	gs_log("gs_sem_close: process counter = %d\n", semval);
#endif

	if (semval > BIGCOUNT) {
		gs_log("gs_sem_close:sem[l] > BIGCOUNT");
		abort();
	}
	else if (semval == BIGCOUNT)
		status = gs_sem_rm(id);
	else {
		status = semop(id, &op_unlock[0], 1);
	}
	return status;
}


 /*
  * Wait until a semaphore's value is greater than 0, then decrement it by 1
  * and return. Dijkstra's P operation. Tanenbaum's DOWN operation.
  */

int     gs_sem_wait(id)
int     id;
{
	return (gs_sem_op(id, -1));
}


 /*
  * Increment a semaphore by 1. Dijkstra's V operation. Tanenbaum's UP
  * operation.
  */
int     gs_sem_signal(id)
int     id;
{
	return (gs_sem_op(id, 1));
}



 /*
  * General semaphore operation. Increment or decrement by a user-specified
  * amount (positive or negative; amount can't be zero).
  */
int     gs_sem_op(id, value)
int     id;
int     value;
{
	int     status = GS_SUCCESS;

	errno = 0;
	if ((op_op[0].sem_op = value) == 0) {
		gs_log("gs_sem_op: can't have value == 0");
		return (GS_FAIL);
	}

	if (semop(id, &op_op[0], 1) < 0)
		status = errno;

	return (status);
}


 /*
  * Dump the values of all the semaphores associated with a given id.
  */
void    gs_sem_dump(str, id)
char   *str;
int     id;
{
	int     sems[3];
	int pid;

	sems[0] = semctl(id, 0, GETVAL, 0);
	sems[1] = semctl(id, 1, GETVAL, 0);
	sems[2] = semctl(id, 2, GETVAL, 0);
	pid = semctl(id,0,GETPID,0);
	gs_log("%s val:%d  processes:%d  lock:%d  lastpid=%lu", str, sems[0], sems[1], sems[2],pid);
	
}

