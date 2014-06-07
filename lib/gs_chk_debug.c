#include "gs.h"
#include <sys/stat.h>

/****************************************************************************
Module: GS_CHK_DEBUG

Description:
	Check if debugging is turned on, and return TRUE if it is, FALSE
	if is not.
	This is agetenv jacket.

Usage:
	int debugflag;
        debugflag = gs_chk_debug();

Parameters:
        INPUT:
			None

        OUTPUT:
			debugflag		- The value of the function is a boolean;

Author:
        Julian Carlisle          Sun May  5 08:42:26 PDT 1991
*****************************************************************************/
extern char *getenv();

static int debugflag = FALSE;

 /*
  * This is just to have a jacket around the getenv statement
  */
int     gs_chk_debug()
{
	static int firsttime = TRUE;

	if (firsttime == TRUE) {
		debugflag = (getenv("GSDEBUG") != NULL) ? TRUE : FALSE;
		firsttime = FALSE;
	}
	return (debugflag);
}

 /*
  * set the debugging state, either TRUE or FALSE.  Returns the previous
  * state of the debugflag.
  */
int     gs_set_debug(mode)
int     mode;
{
	int     savedmode;

	savedmode = debugflag;
	debugflag = (mode == TRUE) ? TRUE : FALSE;
	return (savedmode);
}
