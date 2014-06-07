#include "gs.h"
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/****************************************************************************
    GeoShare Subroutine Library -  Shared Memory Tools.

Module: GS_SHM_ALLOC

Description:
	Allocate a region of shared memory and return a pointer to it.
	The region is created to be at least as big as the size given in the
	parameter list.

	A key is given in the parameter list that is used to identify the
	shared memory segment created.  This key can then be used by other
	programs to attach to the shared memory region.

Usage:
	int id;
	int nbytes;
	GSKey_t *key;
	ptr = gs_shm_alloc(nbytes, key,&id);

	
Parameters:
	INPUT:
		int  nbytes		- Minimum size in bytes
		GSKey_t key		- Memory identifier key.
		int *id			- output id for segment created.

	OUTPUT:
		char *ptr		- Pointer to region allocated.

Status:

	This routine returns a null pointer if an error occurrs while trying
	to create/attach the memory.

Author:
	Julian Carlisle 	 Sun May 12 03:21:10 PDT 1991
*****************************************************************************/
#define GSPOOLSIZE  ((size_t)(1024*1024))
#define GSPOOLKEY  ((key_t)0x06214961)
#define SLOP	(sizeof(int *))

static char *GsFreeend = (char *) 0x3FFFFFFF;
static unsigned int Lastbrk = (unsigned) 0;
int gs_poolid = 0;
int gs_poolsize = 0;
char *gs_poolhead = NULL;
key_t gs_poolkey = 0;

struct fb {
	unsigned int f_size;
	char   *f_next;
};

static struct fb GsFreelist = {
	0,
	(char *) 0x3FFFFFFF,
};

 /* Create if necessary & attach to a shared memory segment */
 /* at least as large as <size>, use the key identifier given */
 /* in key->key */
char   *gs_shm_alloc(size, key, mid)
u_int   size;
GSKey_t *key;
int    *mid;
{
	char   *mp;

	errno = 0;
	*mid = shmget(key->key, size, (IPC_CREAT | 0666));
	if (*mid < 0) {
		fprintf(stderr,"Error: shmget returned %d, errno=%d\n", *mid, errno);
		return (NULL);
	}
	mp = (char *) shmat(*mid, NULL, 0);
	if (mp == (char *) -1) {
		fprintf(stderr,"Error: shmat returned -1, errno=%d\n", errno);
		return (NULL);
	}
	return (mp);
}

/* Detach the shared memory segment pointed to by ptr */
int     gs_shm_free(ptr)
char   *ptr;
{
	int     status = GS_SUCCESS;
	errno = 0;
	status = shmdt(ptr);
	return ((status == 0) ? 0 : errno);
}

int     gs_shm_remove(id)
int     id;
{
	int     status = GS_SUCCESS;
	errno = 0;
	status = shmctl(id, IPC_RMID, NULL);
	return ((status == 0) ? 0 : errno);
}


 /*
  * gs_alloc returns the address of an allocated area in shared memory on
  * success, & NULL if an error occurrs. gs_pool_init must be called prior to
  * calling either gs_alloc or gs_free.
  */
char   *gs_alloc(nbytes)
unsigned nbytes;
{
	register unsigned usize;
	register struct fb *np, *cp;

	if (gs_poolhead == NULL || Lastbrk == 0) {
		fprintf(stderr,"Error: gs_alloc: pool not initialized. Call gs_init_pool()\n");
		return (0);
	}

	if ((usize = nbytes) == 0)
		return (NULL);
	usize += 2 * sizeof(int *) - 1;
	usize &= ~(sizeof(int *) - 1);
	cp = &GsFreelist;
	while ((np = (struct fb *) cp->f_next) != (struct fb *) GsFreeend) {
		if (np->f_size >= usize) {

		/*
		 * Don't break the block up if it is not more than SLOP
		 * bigger than the amount needed.
		 */
			if (usize + SLOP >= np->f_size)
				cp->f_next = np->f_next;

		/*
		 * Break the block into 2 pieces.
		 */
			else {
				cp = (struct fb *) (cp->f_next = (char *) (((int) np) + usize));
				cp->f_size = np->f_size - usize;
				cp->f_next = np->f_next;
				np->f_size = usize;
			}
			return ((char *) &np->f_next);
		}
		cp = np;
	}

 /*
  * Nothing on the free list is big enough;
  */
	fprintf(stderr,"Error: gs_alloc: Out of memory in shared memory pool.\n");
	return (NULL);
}

 /*
  * Free memory allocated by gs_alloc and return it to the freelist.
  */
void     gs_free(aptr)
char   *aptr;
{
	register struct fb *ptr, *cp, *np;

	if (aptr && aptr < (char *) Lastbrk) {
		ptr = (struct fb *) ((unsigned) aptr - sizeof(int *));
		cp = &GsFreelist;
		while ((np = (struct fb *) cp->f_next) < ptr)
			cp = np;

	/*
	 * Try to coalesce with the following block.
	 */
		if (((int) ptr) + ptr->f_size == ((int) np)) {
			ptr->f_size += np->f_size;
			ptr->f_next = np->f_next;
			np = ptr;
		}
		else
			ptr->f_next = (char *) np;

	/*
	 * Try to coalesce with the preceding block.
	 */
		if (((int) cp) + cp->f_size == ((int) ptr)) {
			cp->f_size += ptr->f_size;
			cp->f_next = ptr->f_next;
		}
		else
			cp->f_next = (char *) ptr;
	}
}


 /*
  * This routine creates the shared memory pool for gs_alloc.  You give a
  * size and key for the region to create & initialize.  if size is 0 then
  * the region is created as large as possible.  If the key is 0 then a
  * default key is used.
  */
char *gs_init_pool(size, key)
int     size;
key_t   key;
{
	GSKey_t shmkey;

	register struct fb *cp, *np;
	if (size <= 0 || size > GSPOOLSIZE)
		size = GSPOOLSIZE;

	if (key == (key_t) 0) {
		key = GSPOOLKEY;
	}
	shmkey.key = key;
	gs_poolkey = key;
	gs_poolsize = size;
	gs_poolhead = gs_shm_alloc(GSPOOLSIZE, &shmkey, &gs_poolid);
	Lastbrk = (unsigned) gs_poolhead;
	cp = &GsFreelist;
	cp->f_size = GSPOOLSIZE;
	cp->f_next = (char *) Lastbrk;
	Lastbrk += GSPOOLSIZE - SLOP;
	np = (struct fb *) cp->f_next;
	np->f_size = GSPOOLSIZE;
	np->f_next = GsFreeend;

	return (gs_poolhead);
}

int     gs_term_pool()
{
	int     status = GS_SUCCESS;

	gs_shm_free(gs_poolhead);
	status = gs_shm_remove(gs_poolid);
	if (status == GS_SUCCESS) {
		gs_poolid = 0;
	}
	return (status);
}



#ifdef TEST
void    main()
{
	char   *cp,*cp1,*cp2,*cp3;

	gs_init_pool(0, 0);  /* get max size pool with default key (6214961) */

	printf("InitPool: poolhead = %lu\n", gs_poolhead);

	cp = gs_alloc(5000);
	printf("ptr1 = %lu 		-- should be same as poolhead + 4bytes\n",cp);
	printf("\tFreeing ptr1..\n");
	gs_free(cp);
	cp = gs_alloc(5000);
	printf("ptr2 = %lu 		-- should be same as pointer 1.\n", cp);
	printf("\tFreeing ptr2..\n");
	gs_free(cp);

	cp = gs_alloc(1000);
	printf("ptr = %lu 		-- should be same as pointer 2.\n", cp);

	cp1 = gs_alloc(500000);
	printf("ptr = %lu		-- should be 1000 + last ptr + 4bytes\n",cp1);

	cp2 = gs_alloc(1000);
	printf("ptr = %lu		-- should be 500000 + last ptr + 4bytes\n", cp2);

	cp3 = gs_alloc(100000);
	printf("ptr = %lu		-- should be 1000 + last ptr + 4bytes\n",cp3);

	cp = gs_alloc(1000);
	printf("ptr = %lu		-- should be 100000 + last ptr + 4bytes\n", cp);

	printf("Freeing 500000.. \n");
	gs_free(cp1);

	printf("Freeing 100000.. \n");
	gs_free(cp3);

	printf("allocating 500000.. \n");
	cp = gs_alloc(500000);
	printf("ptr = %lu		-- ptr to 500000 byte region.\n",cp);

	gs_free(cp);
	gs_free(cp2);
	gs_term_pool();

}
#endif
