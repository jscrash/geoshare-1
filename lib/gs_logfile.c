#include "gs.h"
#include <sys/stat.h>

/****************************************************************************
Module: GS_LOGFILE

Description:
	Determine the path of the message logging file for this instance
	of GeoShare.  

	An internally allocated pointer to the log file path is returned. 
	If the logfile argument is not null, the file path is copied into
	the buffer pointed to by logfile.

Usage:
	char *file;
	file = gs_logfile(NULL);
	
Parameters:
	INPUT:
		char *logfile		- optional pointer to user buffer for filename

	OUTPUT:
		NONE


Status:
	A null pointer is returned if the log file path could not be determined
	(i.e. GSHOME not initialized, etc)

Author:
	Julian Carlisle 	 Mon Apr 22 19:54:42 PDT 1991
*****************************************************************************/

extern int getpid();

char   *gs_logfile(logfile)
char  *logfile;
{
	int     len = 0;
	char   *cp;
	static char gserrfile[512];
	struct stat statbuf;


	cp = gs_home(NULL);
	if (cp == NULL) {
		return ((char *) 0);
	}
	(void)sprintf(gserrfile, "%s/%s", cp,GSLOGFILENAME);
	if (!stat(gserrfile, &statbuf)) {
		if (gs_faccess(gserrfile, "w", (char *) 0) != GS_SUCCESS) {
			(void)sprintf(gserrfile, "%s/%d_%s", cp, getpid(),GSLOGFILENAME);
		}
	}
	if (logfile != (char *) 0) {
		len = strlen(gserrfile);
		(void)strcpy(logfile, gserrfile);
	}
	return ((char *) gserrfile);
}
