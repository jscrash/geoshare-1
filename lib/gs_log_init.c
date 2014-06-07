#include "gs.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <ctype.h>


/****************************************************************************
Module: GS_LOG_INIT

Description:
	This routine initializes the error logging system. A error log file
	is created if it does not already exist.  The location of the error
	log varies depending on environment.  The default place is in
	GSHOME/gserrors.log.  If that is not possible, the error log is
	created in the /tmp directory as /tmp/err_<PID>.log


Usage:
	status = gs_log_init();
	gs_log(fmt,args);
	
	Example:
		gs_log("Error: %s  status=%d",msg,status);
	
Parameters:
	INPUT:
		None.
	OUTPUT:
		None.

Status:
 	The returned value will be GS_SUCCESS or GS_FAIL depending on the
	disposition upen completion.

Author:
	Julian Carlisle 	 Wed Apr 17 05:22:31 PDT 1991
*****************************************************************************/


/* Initialize the error message/handling system...  */

char   *gs_log_init(errfile)
char   *errfile;
{
	register int i, status;
	char    *cp;
	static char gserrfile[256];
	static int gs_log_initflag = FALSE;
	FILE   *gserrfp;
	int     gotname, debug_flag;

 /*
  * Since receiver is run as daemon, we don't have a tty.  Open an error log
  * file in the GSHOME directory, name the error log using the client process
  * PID
  */
	debug_flag = gs_chk_debug();

	status = GS_SUCCESS;
	gotname = TRUE;
	if (errfile == (char *) 0) {
		gotname = FALSE;
	}
	else {
	/* Validate that it is a proper string and not an accident. */
		for (i = 0; errfile[i] != '\0'; i++) {
			if (!isprint(errfile[i]))
				gotname = FALSE;
		}
	}

	if (gs_log_initflag == TRUE) {
		if (!gotname) {
			return (gserrfile);
		}
	/* Open the file and write a header ... */
		if ((gserrfp = fopen(errfile, "a+")) > (FILE *)0) {
			fprintf(gserrfp, "\n-- New run Pid=%d --\n", getpid());
			fclose(gserrfp);
			strcpy(gserrfile, errfile);
			status = GS_SUCCESS;
		}
		else {
		/* Attempt to re-initialize with bad filename. */
		/* Leave the previous filename intact & split  */
			status = GS_FAIL;
		}
		return ((char *) (status == GS_SUCCESS) ? gserrfile : (char *) 0);
	}
	else {

		cp = gs_logfile(NULL);
		if (cp == NULL) {
			sprintf(gserrfile,"/tmp/%s",GSLOGFILENAME);
		}
		else {
			strcpy(gserrfile, cp);
		}
	/* Open the file and write a header ... */
		if ((gserrfp = fopen(gserrfile, "a+")) > (FILE *)0) {
			fprintf(gserrfp, "\n--- New run  Pid=%d ---\n", getpid());
			fclose(gserrfp);
			gs_log_initflag = TRUE;
		}
		else {
			fprintf(stderr, "Error opening error file %s\n", gserrfile);
			status = GS_FAIL;
			gs_log_initflag = FALSE;
		}
	}
	return ((status == GS_SUCCESS) ? gserrfile : (char *) 0);
}
