#include "gs.h"
#include <sys/stat.h>
#include <stdarg.h>

/****************************************************************************
Module: GS_LOG

Description:
	This routine provides a way to log messages/errors from a daemon
	process like the GeoShare connection manager.  Daemon processes have
	no controlling terminal so it is necessary to log debug info through
	a psuedo-terminal that is connected to the system console, or
	optionally to a file on disk.

Usage:
	gs_log(fmt,args);
	
	Example:
		gs_log("Error: %s  status=%d",msg,status);
	
Parameters:
	INPUT:
		char *fmt	- printf style format spec.
		void *args,..   - Matching value list for fmt spec.

	OUTPUT:
		None.

Status:
	This is a void function, it has no return value.

Author:
	Julian Carlisle 	 Fri Apr 19 23:02:10 PDT 1991
*****************************************************************************/
#if 0
extern int errno;
extern char *sys_errlist[];
extern int sys_nerr;
#endif 
 /*
  * GS_LOG: Log messages to error log file.  If not connected to an error log
  * then send messages to stderr.
  */
/*VARARGS1*/
int     gs_log(char *fmt, ...)
{

	va_list args;
	int     len, status;
	char   *cp;
	FILE   *fp, *gserrfp;
	static char gserrfile[256];
	static int first_time = TRUE;
	static int debug_flag = FALSE;
	int     myerrno;

	va_start(args,fmt);

	debug_flag = gs_chk_debug();
	myerrno = errno;
	if (first_time == TRUE) {
		if ((cp = gs_log_init(gs_logfile(NULL))) == NULL) {
			status = GS_FAIL;
			if (debug_flag) {
				fp = fopen("/tmp/gs.err", "a+");
				fprintf(fp, "Error initializing logfile.");
				fflush(fp);
				close(fp);
			}
			return (status);
		}
		else {
			first_time = FALSE;
			strcpy(gserrfile, cp);
		}

	}
	if ((gserrfp = fopen(gserrfile, "a+")) <= (FILE *)0) {
		if (debug_flag) {
			fp = fopen("/tmp/cm.err", "a+");
			fprintf(fp, "Error initializing logfile.");
			fflush(fp);
			close(fp);
		}
		status = GS_FAIL;
	}
	else {
		setbuf(gserrfp, NULL);
		cp = va_arg(args, char *);
		vfprintf(gserrfp, fmt, args);
		len = strlen(fmt);
		if (fmt[len - 1] != '\n')
			fprintf(gserrfp, "\n");
		if (myerrno != 0) {
			fprintf(gserrfp, "%d - %s\n", myerrno, strerror(myerrno));
		}
		if (debug_flag) {
			if (myerrno != 0) {
				fprintf(stderr, "%d - %s\n", myerrno, strerror(myerrno));
			}
			vfprintf(stderr, fmt, args);
			if (fmt[len - 1] != '\n')
				fprintf(stderr, "\n");
			fflush(stderr);
		}
		va_end(args);
		status = GS_SUCCESS;
		first_time = FALSE;
		fflush(gserrfp);
		fclose(gserrfp);
	}
	errno = 0;
	return (status);
}
