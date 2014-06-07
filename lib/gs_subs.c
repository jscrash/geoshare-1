#include "gs.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <ctype.h>

/* extern int semop(), semctl(),semget(); */

/*
*  GeoShare API internal routines.
*
* Miscellaneous support routines for the GeoShare Application Programmers
* Interface Library.  These routines are only used internally.
*
* Julian	 Tue Apr 30 02:33:43 PDT 1991
*/

/*
**  Convert an ASCII string of octal digits into an integer
*/
int gsz_octal(str)
char   *str;
{
	register c, num;

	num = 0;
	while ((c = *str++) >= '0' && c <= '7')
		num = (num << 3) + (c - '0');
	return (num);
}

int     gs_semrm(sid)
int     sid;
{
	return (semctl(sid, 0, IPC_RMID, 0));
}

int     gs_semtran(key)
int     key;
{
	int     sid;
	if ((sid = semget((key_t) key, 1, 0666 | IPC_CREAT)) == -1)
		gs_log("gs_semtran: cannot get semaphore");
	return (sid);
}

void    gs_semcall(sid, op)
int     sid;
int     op;
{
	struct sembuf sb;
	int     status;
	sb.sem_num = 0;
	sb.sem_op = op;
	sb.sem_flg = SEM_UNDO;
	errno = 0;
	if (semop(sid, &sb, 1) == -1) {
		status = errno;
		gs_log("gs_semcall: Error from semop.");
		errno = status;
	}
}


void    gs_semP(sid)
int     sid;
{
	gs_semcall(sid, -1);
}

void    gs_semV(sid)
int     sid;
{
	gs_semcall(sid, 1);
}

int     send_data()
{
	return (-1);
}

void    gs_print_args(argc, argv)
int     argc;
char  **argv;
{
	register int i;

	for (i = 0; i < argc; i++)
		fprintf(stderr,"arg%d: %s\n", i, argv[i]);
	fflush(stderr);
}

GSString_t *gs_mk_string(string, pstring_t)
char   *string;
GSString_t *pstring_t;
{
	GSString_t *cp;

	if (pstring_t != (GSString_t *) 0)
		cp = pstring_t;
	else
		cp = (GSString_t *) malloc(sizeof(GSString_t) + 1);

	if (!cp)
		return ((GSString_t *) 0);

	cp->vector_pointer = strdup(string);
	cp->number_elements = strlen(string);
	cp->element_size = sizeof(char);
	cp->element_format = GSV_STRING;
	cp->extension = 0;


	return (cp);
}

GSVector_t *gs_mk_vector(data, dsize, vtype, len, pvec)
char   *data;
unsigned int dsize;
GSVectorType_t vtype;
unsigned int len;
GSVector_t *pvec;
{
	GSVector_t *vp;

	if (pvec != (GSVector_t *) 0)
		vp = pvec;
	else
		vp = (GSVector_t *) malloc(sizeof(GSVector_t) + 1);

	if (!vp)
		return ((GSVector_t *) 0);

	vp->vector_pointer = (GSVoidptr_t) malloc(dsize * len + 1);
	memcpy(vp->vector_pointer, data, len);
	vp->number_elements = len;
	vp->element_size = dsize;
	vp->element_format = vtype;
	vp->extension = 0;

	return (vp);
}



 /*
  * Duplicate a string. Allocate a new pointer.
  */
char   *gs_strdup(cp)
char   *cp;
{
	char   *new;

	if (cp == NULL)
		cp = "";

	new = (char *) malloc((unsigned) (strlen(cp) + 1));
	if (new == (char *) 0) {
		gs_log("Out of memory.");
		return (NULL);
	}
	strcpy(new, cp);
	return (new);
}



/*
 * Since GeoShare must be buildable outside of the Finder SDF environment,
 * this module contains some routines from the Finder Source Library.
 * They have been slightly modified such that they do not require any
 * Finder Header files.
 *
 * This duplication has been done for expediancy, and to avoid re-crafting
 * wheels.
 *
 * JSC 4/30/91
 */


 /*
  * This is a local copy of the standard Finder subroutine ts_tcp_append()
  */
char  **gs_tcp_append(ptcp, cp)
char ***ptcp;
char   *cp;
{
	int     i;
	int     len = 0;
	char  **tmp_tcp;

 /* Search for the null pointer  */

	i = 0;
	if (*ptcp == (char **) NULL || **ptcp == (char *) NULL) {
		tmp_tcp = *ptcp = (char **) malloc((size_t) (sizeof(char *) * 2));
	}
	else {
		tmp_tcp = *ptcp;
		len = gs_tcp_len(tmp_tcp);
		for (i = 0; i < len; i++) {
			if (tmp_tcp[i] == (char *) 0) {
				tmp_tcp[i] = (char *) malloc((size_t) (strlen((char *) cp) + 1));
				strcpy((char *) tmp_tcp[i++], (char *) cp);
				tmp_tcp[i] = (char *) 0;
				return *ptcp;
			}
		}
		*ptcp = tmp_tcp = (char **) realloc((unsigned char *) tmp_tcp,
			(size_t) ((i + 2) * sizeof(char *)));
	}
	tmp_tcp[i] = (char *) malloc((size_t) (strlen((char *) cp) + 1));
	strcpy((char *) tmp_tcp[i++], (char *) cp);
	tmp_tcp[i] = (char *) 0;

	return *ptcp;
}






int     gs_tcp_len(tcp)
char  **tcp;
{
	register int idx = 0;


	if (tcp != (char **) 0)
		while (*tcp++ != NULL)
			idx++;

	return (idx);
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

int     gsz_delete_slot(pslot)
GSSlot_t *pslot;
{
	int     status = GS_SUCCESS;
	int     id = 0;

	errno = 0;
	if (pslot == (GSSlot_t *) 0) {
		gs_log("gs_delete_slot: Null slot pointer.");
		return (GS_FAIL);
	}
	gs_semrm(pslot->locksem);
	gs_semrm(pslot->msgsem);
	gs_semrm(pslot->cmsem);

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


int     gsz_detach_slot(pslot)
GSSlot_t *pslot;
{

	errno = 0;
	if (pslot == (GSSlot_t *) 0) {
		gs_log("gs_detach_slot: Null slot pointer.");
		return (GS_FAIL);
	}

	gs_semrm(pslot->locksem);
	gs_semrm(pslot->msgsem);
	gs_semrm(pslot->cmsem);

	if (gs_shm_free((char *)pslot) < 0) {
		gs_log("gs_detach_slot:Cannot detach shared memory %d", pslot);
		return (GS_FAIL);
	}

	return (GS_SUCCESS);
}

 /*
  * Here is the AT&T public domain source for getopt(3). 
  * It is the code which was given out at the 1985 UNIFORUM conference
  * in Dallas.  I obtained it by electronic mail directly from AT&T.
  * The people there assure me that it is indeed in the public domain.
  */

/*LINTLIBRARY*/
#define NULL	0
#define EOF	(-1)
#define ERR(s, c)	if(opterr){\
	char errbuf[2];\
	errbuf[0] = c; errbuf[1] = '\n';\
	(void) write(2, argv[0], (unsigned)strlen(argv[0]));\
	(void) write(2, s, (unsigned)strlen(s));\
	(void) write(2, errbuf, 2);}

extern int strcmp();
extern char *strchr();

int     opterr = 1;
int     optind = 1;
int     optopt;
char   *optarg;

int     getopt(argc, argv, opts)
int     argc;
char  **argv, *opts;
{
    static int sp = 1;
    register int c;
    register char *cp;

    if (sp == 1)
	if (optind >= argc ||
	    argv[optind][0] != '-' || argv[optind][1] == '\0')
	    return (EOF);
	else if (strcmp(argv[optind], "--") == NULL) {
	    optind++;
	    return (EOF);
	}
    optopt = c = argv[optind][sp];
    if (c == ':' || (cp = strchr(opts, c)) == NULL) {
	ERR(": illegal option -- ", c);
	if (argv[optind][++sp] == '\0') {
	    optind++;
	    sp = 1;
	}
	return ('?');
    }
    if (*++cp == ':') {
	if (argv[optind][sp + 1] != '\0')
	    optarg = &argv[optind++][sp + 1];
	else if (++optind >= argc) {
	    ERR(": option requires an argument -- ", c);
	    sp = 1;
	    return ('?');
	}
	else
	    optarg = argv[optind++];
	sp = 1;
    }
    else {
	if (argv[optind][++sp] == '\0') {
	    sp = 1;
	    optind++;
	}
	optarg = NULL;
    }
    return (c);
}
