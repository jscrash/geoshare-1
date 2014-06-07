#include "gs.h"
/****************************************************************************
Module: GS_IPC_KEY

Description:
	This routine will return a key for opening shared resources that are
	identified by a 'well known key/address'.  The input filename is
	converted to a unique integer key by using the ftok() function.
	
	If the environment variable GSCMKEY is defined, that value will be
	used to generate the key.  This overrides the file path in the
	parameter list and should be explicitly undefined if this behaviour
	is not desired.

	This function may be called with a null file path if the GSCMKEY
	environment variable is defined.


Usage:
	key = (GSKey_t *) gs_ipc_key("/home/julian/some.file");

Parameters:
	INPUT:
		char *filename		- Full path of file to key from

	OUTPUT:
		None.  Returns a pointer to a GSKey_t key struct as value
		of function.
Status:

	On error, such is the given file not existing, a null key pointer is
	returned (GSKey_t *)0.
	

Author:
	Julian Carlisle 	 Mon Apr 22 23:34:40 PDT 1991
*****************************************************************************/

GSKey_t *gs_ipc_key(file)
char   *file;
{
	char   *cp;
	GSKey_t *retkey;



	cp = getenv("GSCMKEY");
	if (file == NULL) {
		if (cp == NULL)
			return ((GSKey_t *) 0);
	}



	retkey = (GSKey_t *) malloc(sizeof(GSKey_t) + 1);

	if (cp == (char *) 0) {
	/* No ipc key Env variable defined. */
	/* Use the given file path instead. */
		retkey->str = (char *) malloc(strlen(file) + 1);
		strcpy(retkey->str, file);
		retkey->key = (key_t) ftok(file, FTOKID);
	}
	else {
		retkey->str = (char *) malloc(strlen(cp) + 1);
		strcpy(retkey->str, cp);
		retkey->key = (key_t) atoi(cp);
	}
	if (retkey->key == (key_t) - 1) {
		free(retkey->str);
		free(retkey);
		retkey = (GSKey_t *) 0;
	}

	return (retkey);
}
