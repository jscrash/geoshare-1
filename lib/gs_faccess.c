/*
* gs_faccess - test accessability of a file by a given user
*
* JSC
*/
#include "gs.h"
#include <string.h>
#include <pwd.h>
#include <sys/stat.h>

#define USR 	05700	     /* users bits include set-uid, sticky */
#define GRP 	02070	     /* group bits include set-gid */
#define OTH 	00007
#define ALL 	00777

#define READ 	00444
#define WRITE 	00222
#define EXEC    00111
#define SETID   06000
#define STICKY  01000

static int setperms();

int     gs_faccess(file, mode, arg)
char   *file;
char   *mode;
char   *arg;
{
	char   *path = NULL; /* pathname of file to be checked */
	char   *name = NULL; /* optional name of user */
	char    permstr[32], *pp;	/* perms on command line */
	int     perms = 0;   /* permissions default to r */
	int     setwho = 1;  /* set who via -ugo, FALSE with -n */
	int     uid = 0;
	int 	gid = 0;
	int     i = 0;
	int     chkown = 0;
	int     chkgrp = 0;
	int     verbose = 0;
	int     done = 0;
	char   *owner = NULL;
	char   *group = NULL;
	struct stat statbuf;
	struct passwd *pass;
	struct passwd *getpwnam();

	path = file;
	done = 0;
	chkown = 0;
	chkgrp = 0;
	verbose = 0;
	pp = &permstr[0];

	for (i = 0; done == 0 && mode[i] != '\0'; i++) {
		switch (mode[i]) {
		case ('O'):
			chkown = 1;
			owner = arg;
			done = 1;
			break;
		case ('G'):
			chkgrp = 1;
			group = arg;
			done = 1;
			break;
		case ('v'):
			verbose = 1;
			break;
		case ('n'):
			name = arg;
			setwho = 0;
			done = 1;
			break;
		case ('r'):
		case ('w'):
		case ('x'):
		case ('s'):
		case ('t'):
		case ('u'):
		case ('g'):
		case ('o'):
		case ('a'):
			*pp++ = mode[i];
			break;
		default:
			return (GS_FAIL);
		}
	}
	*pp = '\0';

	if (path == NULL) {  /* pathname is mandatory */
		return (GS_FAIL);
	}

	if ((perms == 0) &&  /* if no numeric perms, try letters */
		(chkown == 0) &&
		(chkgrp == 0))
		if ((perms = setperms(permstr, setwho)) == GS_FAIL) {
			return (GS_FAIL);
		}

	if (name != NULL) {  /* if user specified, set uid,gid */
		if ((pass = getpwnam(name)) == NULL) {
			return (GS_FAIL);
		}
		uid = pass->pw_uid;
		gid = pass->pw_gid;

		if (setgid(gid) < 0) {	/* set group ID first, while UID root */
			return (GS_FAIL);
		}
		if (setuid(uid) < 0) {	/* now set user ID */
			return (GS_FAIL);
		}
	}
	if (stat(path, &statbuf) < 0) {	/* stat the file */
		return (GS_FAIL);
	}
	if (chkown) {
		if ((pass = getpwnam(owner)) == NULL) {
			return (GS_FAIL);
		}
		if (statbuf.st_uid == pass->pw_uid)
			return (GS_SUCCESS);
		else {
			return (GS_FAIL);
		}
	}
	if (chkgrp) {
		if ((pass = getpwnam(group)) == NULL) {
			return (GS_FAIL);
		}
		if (statbuf.st_gid == pass->pw_gid)
			return (GS_SUCCESS);
		else {
			return (GS_FAIL);
		}
	}

	if (name != NULL) {  /* if who specified by name *//* check how user
			      * can access file */
		if (statbuf.st_uid == uid)
			perms &= USR;
		else if (statbuf.st_gid == gid)
			perms &= GRP;
		else
			perms &= OTH;
	}

	if ((statbuf.st_mode & perms) != perms) {
		return (GS_FAIL);
	}
	return (GS_SUCCESS);
}

/*
**  Given a string of characters '[rwxstugoa]*', return the corresponding
**   set of permissions.  e.g. urx (User Read and eXecute) returns 00500
*/
static int setperms(perm, setwho)
char   *perm;
int     setwho;
{
	int     i;
	int     tperms = 0;
	int     whomask = 0;

	for (i = 0; i < strlen(perm); i++)
		switch (perm[i]) {
		case ('r'):
			tperms |= READ;
			break;
		case ('w'):
			tperms |= WRITE;
			break;
		case ('x'):
			tperms |= EXEC;
			break;
		case ('s'):
			tperms |= SETID;
			break;
		case ('t'):
			tperms |= STICKY;
			break;
		case ('u'):
			whomask |= USR;
			break;
		case ('g'):
			whomask |= GRP;
			break;
		case ('o'):
			whomask |= OTH;
			break;
		case ('a'):
			whomask |= ALL;
			break;
		default:
			printf("bad %x\n", perm[i]);
			return (GS_FAIL);
		}
	if (tperms == 0)
		tperms = READ;	/* permissions default to read */

	if (whomask == 0)
		whomask = USR;	/* who defaults to user */

	if (setwho)
		return (tperms & whomask);
	else
		return (tperms);
}
