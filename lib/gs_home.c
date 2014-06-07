#include "gs.h"
#include <sys/stat.h>

/****************************************************************************
Module: GS_HOME

Description:

     Get the homedirectory specification from the environment,
     assign one if none is defined.

     Determine the location of the GeoShare product home directory using the
     following guidlines:

          * Check for GSHOME environment variable, use it if defined as
            a valid path.

          * If GSHOME is not defined, use the definition of GSDEFAULTHOME

          * If neither of the above yields a valid home dir spec., log the
            error message and return NULL.


Usage:
	char *homedir;

        homedir = gs_home(&homedir);

Parameters:
        INPUT:
		None

        OUTPUT:
		char **homedir		- Adress of char ptr to allocate.

Status:
	This routine returns a NULL pointer if an error occurs.  A message
	will be printed if the environment variable GSDEBUG is defined.


Author:
        Julian Carlisle          Fri Apr 20 06:09:42 PDT 1991
*****************************************************************************/

extern char *getenv();
extern char *getcwd();


char   *gs_home(homedir)
char   *homedir;
{
	int     status;
	int     debug = FALSE;
	int     len = 0;
	char   *cp = (char *) 0;
	char    home[512];
	char    text[512];
	struct stat statbuf;


	debug = gs_chk_debug();
	status = GS_SUCCESS;

 /* getcwd(cwd, sizeof cwd); */

 /*
  * Figure out where the GeoShare home directory is, if it is not specified
  * anywhere then set it to the default.
  */
	if ((cp = (char *) getenv("GSHOME")) != NULL) {
		strcpy(home, cp);
	}
	else {
		strcpy(home, GSDEFAULTHOME);
		sprintf(text,"%s/gstab",home);
		if (stat(text, &statbuf)) {
			status = GS_FAIL;
		}
		if (debug) {
			gs_log("gs_home:  Warning: GSHOME not defined.  Using %s", home);
		}

	}

 /* Check if whatever we got exists... */
	if (stat(home, &statbuf)) {
		gs_log("gs_home: Can't locate path: %s\n", home);
		status = GS_FAIL;
	}

 /* Check for permission to work in here... */
	if (gs_faccess(home, (char *) "rw", (char *) 0)) {
		if (debug)
			gs_log("gs_home: Directory %s is not r/w accessible.", home);
		status = GS_FAIL;
	}

	if (status == GS_SUCCESS && (homedir != (char *) 0)) {
		len = strlen(home);
		strcpy(homedir, home);
	}

	if (status == GS_SUCCESS)
		return (home);
	else
		return ((char *) 0);
}
