#include "gs.h"
#include <signal.h>
#include <stdarg.h>
#include <ctype.h>

/***********************************************************
*  GS_TEST  - Test harness for GeoShare library routines.
*
* Julian    Wed May  1 01:25:26 PDT 1991
***********************************************************/

static void gs_log();
static void gs_cleanup();
static int silent = FALSE;
static int debug = FALSE;

extern char *gs_poolhead;
extern key_t gs_poolkey;
extern char *optarg;
extern int optind;
extern int putenv();


void main(argc, argv)
int     argc;
char  **argv;
{
	register int i, j;
	int     status = GS_SUCCESS;
	int     len = 0;
	int     gotit = 0;
	int     done = FALSE;
	char    context[256];
	char    text[512];
	char    recname[256];
	char   *cp = NULL;
	char   *cp2 = NULL;
	char  **tcp = (char **) 0;
	GSHandle_t *rech;
	GSContextRequirements_t **cr;
	char    c;
	GSMessage_t result;
	GSTCB_t *ptcb;
	unsigned int count = 0;

	errno = 0;
	debug = FALSE;
	silent = FALSE;


	while ((c = getopt(argc, argv, "dsh:")) != -1) {
		switch (c) {
		case 'd':
			debug = TRUE;
			break;

		case 'h':
			sprintf(text,"GSHOME=%s",optarg);
			putenv(text);
			break;

		case 's':
			silent = TRUE;
			break;

		case '?':
			gs_log("Usage:\n\t%s [-d]\n", argv[0]);
			exit(1);
		default:
			gs_log("Impossible argument parsing in getopt.i\n");
			exit(1);
		}
	}

	cp = gs_home(NULL);
	if (cp == NULL) {
		gs_log("Error: GSHOME not defined.\n");
		gs_log("You must 'setenv GSHOME /j/fl/lib' before running this.\n");
		exit(1);
	}

	if (debug) {
		gs_log(" \n");
		gs_log(" 		GS_TEST - test geoshare library\n");
		gs_log(" \n");
		gs_log("This test program inquires all of the available data receivers\n");
		gs_log("known to the Connection Manager and lets you select one.\n");
		gs_log("The context arguments for that receiver are inquired from the\n");
		gs_log("Connection Manager and you are prompted to fill them in.\n");
		gs_log("The selected receiver is started with the arguments given.\n");
		gs_log(" \n");
	}

	signal(SIGINT,gs_cleanup);
	signal(SIGHUP,SIG_IGN);
	if (debug) {
		gs_log("Looking for Connection Manager Daemon...\n");
		if (check_cm() == GS_SUCCESS) {
			gs_log("Shutting down current connection manager process.\n");
			term_cm();
		}
		gs_log("Starting connection manager process....\n");
		start_cm();
	}
/*
* Call GS_INQUIRE_DESTINATIONS to get list of all receivers.
*/
	status = gs_inquire_destinations(&tcp,&count);
	if (status != GS_SUCCESS) {
		gs_log("Test: Error from gs_inquire_destinations. %d\n", status);
		gs_log("  --- Is the Connection Manager (gs_cm) running?\n");
		gs_log(" To start the connection manager, type:  gs_cm\n\n");
		exit(1);
	}

	gs_log("%u destinations available \n",count);
	done = FALSE;
	while (!done) {
		gs_log("\n\nThe following data receivers are available:\n");
		len = gs_tcp_len(tcp);
		for (j = 0; j < len; j++) {
			if (tcp[j] != NULL) {
				gs_log("\t\t%s\n", tcp[j]);
			}
		}
		text[0] = '\0';
		gs_log("Select a receiver to start: (<RETURN> or <q> to quit) ");
		gets(recname);
		if (!strlen(recname) || !strcmp(recname, "q")) {
			done = TRUE;
			continue;
		}

		cp = recname;
		while (!isalnum(*cp))
			cp++;

		cp2 = cp;
		while (isalnum(*cp2))
			cp2++;
		*cp2 = '\0'; /* Trim off trailing blanks */
		if (cp != recname) {
			len = strlen(cp);
			if (len != 0) {
				memcpy(recname, cp, len + 1);
			}
			else {
				gs_log("Invalid receiver name.\n");
				continue;
			}
		}
		gotit = FALSE;

/*
* Find the input name in the list of recievers..
*/
		len = gs_tcp_len(tcp);
		for (j = 0; j < len; j++) {
			if (tcp[j] != NULL) {
				if (strcmp(tcp[j], recname) == 0) {
					gotit = TRUE;
					break;
				}
			}
		}
		if (!gotit) {
			gs_log("\n%s is not a valid choice.\n", recname);
			continue;
		}

/*
* Call  GS_INQUIRE_CONTEXT to get needed arguments for receiver
*/
		text[0] = '\0';
		status = gs_inquire_context(recname, &cr,&count);
		if (status != GS_SUCCESS) {
			gs_log("test: inq context failed %d\n", status);
		}
		else if (count > 0) {

/*
* For each context arg given, prompt the user for value and assign it.
*/
			gs_log("%u context args \n",count);
			gotit = TRUE;
			len = 0;
			cp = text;
			for (i = 0; cr[i]; i++) {
				context[0] = '\0';
				gs_log("\tEnter %s: ",
					cr[i]->prompt.vector_pointer);
				gets(context);
				if (strlen(context) == 0) {
					gs_log("All arguments must be entered.... Aborting.\n");
					gotit = FALSE;
					break;
				}

				sprintf(cp, " %s %s",
					cr[i]->cmdline_tag.vector_pointer,
					context);
				cp += strlen(cp);
			}
			if (!gotit) {
				gs_log("\n");
				continue;
			}
		}
/*
* Execute the receiver program
*/
		gs_init_pool(0,0);
		gs_log("\nStarting:  %s %s\n", recname, text);
		rech = gs_initialize(recname, text,&status);
		if (rech == (GSHandle_t *)0 || status != GS_SUCCESS) {
			switch(status) {
			case GSERR_NOCONNECT:
				gs_log("Error:  Connect attempt failed.\n");
				break;
			case GSERR_MSGTYPE:
				gs_log("Error: Unknown Message Type from CM. Connect failed.\n");
				break;
			case GSERR_CM_NOTINIT:
				gs_log("Error: Connection Manager not initialized.\n");
				break;
			default:
				gs_log("Error starting receiver ... Check %s for details\n",
					gs_cf_name(NULL));
				break;
			}
			continue;
		}

		ptcb = (GSTCB_t *)gs_alloc(sizeof(GSTCB_t));
		ptcb->BlockNum = 1;
		printf("calling send.\n");
		gs_send(rech,ptcb,&result);

		ptcb = (GSTCB_t *)gs_alloc(sizeof(GSTCB_t));
		ptcb->BlockNum = 2;
		printf("calling send again.\n");
		gs_send(rech,ptcb,&result);

		printf("calling sendstr.\n");
		gs_sendstr(rech,  "This is a test");

		printf("calling sendstr.\n");
		gs_sendstr(rech, "This is also test");

		printf("calling sendstr.\n");
		gs_sendstr(rech, "This is the final test");
		printf("calling terminate.\n");
		gs_terminate(rech);
		gs_term_pool();
		sleep(0);
		errno = 0;
	}
#if 0
	gs_detach_slot(rech->cmslot);
	gs_term_pool();
#endif
	if (debug) {
		gs_log("\nShutting down Connection Manager Daemon.\n");
		term_cm();
	}
	gs_log("Done.\n");
	exit(0);
	/*NOTREACHED*/
}



int     start_cm()
{
	int     status = GS_SUCCESS;

	errno = 0;
	status = system("gs_cm");
	if (status) {
		gs_log("Error starting Connection Manager\n");
		status = GS_FAIL;
	}
	else {
		gs_log("Connection Manager Daemon Started.\n");
		status = GS_SUCCESS;
	sleep(1);
	}
	return (status);
}

int     term_cm()
{
	int     status = GS_SUCCESS;
	system("kill -HUP `ps ax | grep gs_cm | grep -v grep | awk '{print $1}'`");
	status = system("ps ax | grep gs_cm | grep -v grep >/dev/null");
	if (status == 0) {
		gs_log("Warning: Cannot kill Connection Manager Daemon. (gs_cm)\n");
		exit(1);
	}
	status = GS_SUCCESS;
	sleep(1);
	return (status);
}

int     check_cm()
{
	int     status = GS_SUCCESS;
	errno = 0;
	status = system("ps ax | grep gs_cm | grep -v grep >/dev/null");
	if (status == 0) {
		status = GS_SUCCESS;
	}
	else {
		status = GS_FAIL;
	}
	return (status);
}






static void gs_cleanup()
{
static int first = TRUE;
int status = 0;
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,gs_cleanup);

	if (first == TRUE) {
		printf("\nCleaning up...\n");
		first = FALSE;
		signal(SIGALRM,gs_cleanup);
		alarm(2);
		gs_terminate((GSHandle_t *)gs_get_handle());
	}
	else {
		printf("\nCleaning up. (Brute force this time)\n");
		status = gs_delete_slot(((GSHandle_t *) gs_get_handle())->slot);
	}
	alarm(0);
	gs_term_pool();
	exit(0);
}
