#include "gs.h"
#include <sys/stat.h>

/****************************************************************************
Module: GS_CF_NAME

Description:
     Returns the name of the configuration file used by the GeoShare
     connection manager.

     Determine the location of the gstab configuration file using
     the following guidelines:

        * The default config file is  $GSHOME/gstab

        * The environment variable GSCONFIG is search for, and if found
          supersedes the default spec.

        * If none of the above yields a valid config file spec a NULL
	pointer is returned. If the environment variable GSDEBUG is
	defined an error message is printed.


Usage:
        config_file = (char *)gs_cf_name(config_file);
        char **config_file;

Parameters:
        INPUT:
		None
        OUTPUT:
		char ** config_file;  	- ptr to char pointer to allocate

Status:
	a NULL pointer is returned if an error occurs.  Otherwise a pointer
	is returned to an internally allocated string buffer containing the
	name of the configuration file.

Author:
        Julian Carlisle        Sun Apr 28 20:18:12 PDT 1991
*****************************************************************************/
extern char *getenv();
extern char *gs_home();

char   *gs_cf_name(cfgfile)
char  *cfgfile;
{
	int     status = GS_SUCCESS;
	int     debug = FALSE;
	int     len = 0;
	char   *cp;
	static char config[256];
	static int firsttime = TRUE;
	struct stat statbuf;



	if (firsttime == FALSE) {
		if (cfgfile != (char *) 0) {
			len = strlen(config);
			strcpy(cfgfile, config);
		}
		return (config);
	}
	debug = gs_chk_debug();

 /*
  * Figure out where the GeoShare home directory is, if it is not specified
  * anywhere then set it to /tmp.
  */
	if ((cp = (char *) getenv("GSCONFIG")) != (char *) 0) {
		strcpy(config, cp);
	}
	else {
		cp = (char *) gs_home(NULL);
		if (cp == NULL)
			return ((char *) 0);
		sprintf(config, "%s/gstab", cp);
	}

 /* Check if whatever we got exists... */
	if (stat(config, &statbuf)) {
		if (debug)
			gs_log("gs_config_file: Can't find gstab file %s\n", config);
		status = GS_FAIL;
	}

	if (status == GS_SUCCESS && (cfgfile != (char *) 0)) {
		len = strlen(config);
		strcpy(cfgfile, config);
	}

	if (status == GS_SUCCESS) {
		firsttime = FALSE;
		return (config);
	}
	else
		return ((char *) 0);

}
