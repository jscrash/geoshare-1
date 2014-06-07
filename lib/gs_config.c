#include "gs.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

/****************************************************************************
Module: GS_CONFIG

Description:
	This is a toolbox of routines that manipulate the GeoShare
	connection manager configuration file. (gstab)

	The configuration file format is modelled from the Unix Termcap file
	format.  Each new entry consists of one or more 'names' for the
	entry followed by the specifications for the entry.  Each field in an
	entry is of the following format:
		:xxxx=yyyyy:
	where xx is a character field identifier, and yyyy is a string
	of variable length terminated by a colon.
	Note that every entry in the file must begin and end with a ':' .
	If an entry continues for several lines, each line must be
	terminated with a '\' character.

Status:

	GS_SUCCESS & GS_FAIL


Author:
	Julian Carlisle 	 Thu Apr 18 01:42:39 PDT 1991
*****************************************************************************/
#ifndef TEST
#define TEST    0
#else
#undef  TEST
#define TEST 	1
#endif


#define	BUFSIZE	2048

extern char *getenv();

static char *ebuf = NULL;    /* pointer to entry buffer */

/*
 * Returns pointing to the character immediately following the capability id.
 * Returns NULL if gs_cgetent() has not yet been called successfully.
 * Returns NULL if capability not found.
 */
static char *gs_findCap(id)
char   *id;		     /* name of the capability to find */
{
	char   *p;	     /* pointer into the entry buffer */
	register char *p1;
	register int i;

	if (ebuf == NULL)
		return NULL;
	if (id == (char *) 0 || id[0] == '\0')
		return NULL;

	for (p = ebuf; *p; ++p) {
		if (p[0] == ':' && p[1]) {
			for (p1 = p + 1, i = 0; id[i]; i++) {
				if (p1[i] == '\0' || id[i] != p1[i])
					break;
			}
			if (id[i])
				continue;
			else {
				p = p1 + i;
				break;
			}
		}
	}
	if (!*p)
		p = NULL;
	return p;
}



/*
 * Gets the named entry from the already-opened gstab file into the buffer.
 * The size of the buffer is BUFSIZE, and it is considered to be an
 * error if the size of the entry equals or exceeds this.
 * We place a terminating NULL character at the end of the entry.
 * Call gs_log() on any irregularities.
 * Return 0 if the named entry not found, else 1.
 * Removes host names and all newlines from the entry.
 **If this is called for a 2nd time from gs_cgetent(), then the length checking
 **is useless.
 */
static int gs_getEntry(fd, outbuf, name)
int     fd;		     /* FileDescriptor for gstab file */
char   *outbuf;		     /* where we put the entry */
char   *name;		     /* host name we seek */
{
	unsigned namlen;     /* # chars in name */
	int     cnt;	     /* # unprocessed chars in inbuf[] */
	char   *ip;	     /* pointer into input buffer */
	char   *op;	     /* pointer into output buffer */
	char   *ptmp;	     /* temporary pointer */
	int     stat;	     /* status of read(), etc */
	char    inbuf[BUFSIZE];	/* gstab file is read into here */

	op = outbuf;
	namlen = strlen(name);
	if (outbuf == NULL) {
		gs_log("gs_getEntry: outbuf is NULL");
		return (GS_FAIL);
	}
	if (name == NULL || namlen == 0 || fd < 0) {
		return (GS_FAIL);
	}

	if ((cnt = read(fd, inbuf, BUFSIZE - 1)) < 0) {
		gs_log("gs_getEntry(): file is empty\n");
		return (FALSE);
	}

	inbuf[cnt] = '\0';   /* maintain inbuf[] as a string */
	for (ip = inbuf; 0 < cnt; ++ip, --cnt) {
		stat = strspn(ip, "\r\n \t\b\f");
		if (0 < stat) {
			ip = &ip[--stat];
			cnt -= stat;
		}
		else if (*ip == '#') {
			ptmp = ip;
			ip = strchr(ip, (char) '\n');
			cnt -= (ip == NULL) ? cnt : (int) (ip - ptmp);
		}
		else if (strncmp(name, ip, namlen) == 0) {
		/* skip over namelist */
			ip = strchr(ip, (char) ':');
		/* copy to output buffer, removing */
		/* non-space whitespace and \ chars */
			for (op = outbuf; ip != NULL && *ip != '\0'; ++ip) {
				if (ip[0] == '\\' && ip[1] == '\r' && ip[2] == '\n')
					ip = &ip[2];
				else if (ip[0] == '\\' && ip[1] == '\n')
					++ip;
				else if (strchr("\t\r\b\f", *ip) != NULL)
					continue;
				else if (*ip == '\n')
					break;
				else
					*op++ = *ip;
			}
			if (*ip != '\n')
				gs_log("gs_getEntry(): entry too long\n");
			*op = '\0';
			return (TRUE);
		}
		else {	     /* advance to next name in list */
			ptmp = ip;
			ip = strpbrk(ip, "|:");	/* find name delimiter */
			if (ip == NULL)
				gs_log("gs_getEntry(): bad format\n");
			cnt -= ip - ptmp;
			if (*ip != '|') {	/* at end of namelist for
						 * entry */
			/* dispose of entire entry */
				for (++ip, --cnt; 0 < cnt; ++ip, --cnt)
					if (ip[0] == '\n' &&
						((ip[-1] == '\r' && ip[-2] != '\\') ||
							(ip[-1] != '\r' && ip[-1] != '\\'))) {
					/* skip to next entry in file */
					/* delete this entry from inbuf */
						for (ptmp = inbuf; *ip != '\0'; ++ptmp, ++ip)
							*ptmp = *ip;
						*ptmp = *ip;	/* string stopper
								 * character */
						ip = inbuf;
						if (strlen(ip) != cnt)
							gs_log("gs_getEntry(): bad strlen(ip)\n");

					/* fill inbuf with more characters */
						stat = read(fd, ptmp, BUFSIZE - cnt - 1);
						if (0 < stat) {
							cnt += stat;
							inbuf[cnt] = '\0';
						}
						break;
					}
				if (cnt <= 0)
					gs_log("gs_getEntry(): entry too long!\n");
			}
		}
	}
	outbuf[0] = '\0';    /* not found */
	return FALSE;
}



/*
 * Extracts the entry for host name into the buffer at bp.
 * Bp should be a character array of size 1024 and must be retained through
 * all subsequent calls to gs_cgetnum(), gs_cgetflag(), and gs_cgetstr().
 * Returns GS_FAIL if it cannot open the gstab file, 0 if the host name
 * given does not have an entry, and 1 if all goes well.
 */
int     gs_cgetent(bp, name)
char   *bp;		     /* pointer to user's buffer */
char   *name;		     /* host name */
{
	int     fd;	     /* File Descriptor, gstab file */
	int     retval;	     /* return value */
	char   *cp;
	static int first_time = TRUE;
	struct stat statbuf;
	static char gstab[256];	/* pointer to string */

	if (first_time == TRUE) {
		cp = gs_cf_name(gstab);
		if (cp == NULL) {
			cp = gs_home(NULL);
			if (cp == NULL) {
				gs_log("Can't find GeoShare config file directory.\n");
				return (GS_FAIL);
			}
			sprintf(gstab, "%s/gstab", cp);
		}
		if (stat(gstab, &statbuf)) {
			gs_log("Cannot locate config file %s", gstab);
			return (GS_FAIL);
		}
		first_time = FALSE;
	}

	fd = open(gstab, O_RDONLY);
	if (fd == -1)
		retval = GS_FAIL;
	else {
		retval = gs_getEntry(fd, bp, name);
		close(fd);
	}
	if (retval == TRUE)
		ebuf = bp;   /* for our use in future pkg calls */
 /* deal with the :include= capability */
	bp = gs_findCap("include");
	if (bp != NULL) {
		char    newname[88];

		strncpy(newname, &bp[1], sizeof newname);
		if (strchr(newname, (char) ':') != NULL)
			*(strchr(newname, (char) ':')) = '\0';
		fd = open(gstab, O_RDONLY);
		if (fd == -1) {
			gs_log("gs_cgetent(%s): can't open :include file '%s'\n",
				name, newname);
			retval = GS_FAIL;
		}
		else {
			retval = gs_getEntry(fd, &bp[-7], newname);
			close(fd);
		}
	}

	return retval;
}



/*
 * Gets the numeric value of capability id, returning GS_FAIL if it is not given
 * for the host.
 */
int     gs_cgetnum(id)
char   *id;		     /* capability name */
{
	int     retval;
	char   *p;

	p = gs_findCap(id);
	if (p == NULL || *p != '#')
		retval = GS_FAIL;	/* not found, or not numeric */
	else {
		retval = 0;
		for (++p; *p != ':'; ++p)
			retval = (retval * 10) + (*p - '0');
	}
	return retval;
}



/*
 * Returns 1 if the specified capability is present in the host's entry,
 * 0 if it is not.
 **This implementation requires that the capability be a boolean one.
 */
int     gs_cgetflag(id)
char   *id;		     /* capability name */
{
	int     retval;
	char   *p;

	p = gs_findCap(id);
	retval = (p != NULL && *p == ':');
	return retval;
}



/*
 * Returns the string value of the capability id, places it in the buffer
 * at area, and advances the area pointer [past the terminating '\0' char].
 * Returns NULL if the capability was not found.
 */
char   *gs_cgetstr(id, area)
char   *id;		     /* capability name */
char  **area;		     /* pointer to output pointer */
{
	char   *retval;	     /* return value */
	char   *p;	     /* pointer into capability string */
	unsigned sum;	     /* for chars given in octal */

	p = gs_findCap(id);
	if (p == NULL || *p != '=')
		retval = NULL;
	else {
		retval = *area;
		for (++p; *p != ':'; ++p) {
			if (*p == '\\')
				switch (*++p) {	/* special */
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					sum = ((p[0] - '0') << 6) + ((p[1] - '0') << 3) + (p[2] - '0');
					++p;
					++p;
					*(*area)++ = (char) (sum & 0377);
				/** will \200 really end up as \000 like it should ? **/
					break;
				case '^':
					*(*area)++ = '^';
					break;
				case '\\':
					*(*area)++ = '\\';
					break;
				case 'E':
					*(*area)++ = '\033';
					break;	/* escape */
				case 'b':
					*(*area)++ = '\b';
					break;
				case 'f':
					*(*area)++ = '\f';
					break;
				case 'n':
					*(*area)++ = '\n';
					break;
				case 'r':
					*(*area)++ = '\r';
					break;
				case 't':
					*(*area)++ = '\t';
					break;
				default:
					*(*area)++ = *p;
					break;
				}
			else if (*p == '^')
				*(*area)++ = *++p - '@';	/* control */
			else
				*(*area)++ = *p;	/* normal */
		}
		*(*area)++ = '\0';	/* NULL-terminate the string */
	}
	return (retval);
}




#if TEST

/*****************************************************
 *
 * Test program for the gstab routines.
 * Command line parameters:
 *		1st is host name, defaulted to "snuffy".
 *		2nd is name of numeric capability
 *		3rd is name of boolean capability
 *		4th is name of string capability
 *
 *****************************************************/

static char buf[1024];	     /* holds gstab entry */
static char strbuf[512];     /* for output of gs_cgetstr() */
static char *strptr;	     /* ptr to strbuf[] */
extern char *optarg;
extern int optind;
extern char getopt();
static char **av;
static void usage();


int     main(argc, argv)
int     argc;
char  **argv;
{
	int     status = GS_SUCCESS;
	int     ac, len = 0;
	int     debug = 0;
	char    text[512];
	char   *instr;	     /* input string value */
	char   *outstr;	     /* string return value */
	char   *htype;	     /* host string */
	char   *capability;  /* capability name string */
	char   *cp = NULL;
	char   *cp2 = NULL;
	char    c;

	av = argv;
	ac = 0;
	htype = NULL;

	if (argc == 1) {
		usage();
		exit(1);
	}

	cp = gs_home(0);
	if (cp == NULL) {
		printf("Error: Must define GSHOME as dir where gstab file lives.\n");
		exit(1);
	}

	status = GS_FAIL;
	htype = argv[1];

	if (*htype != '-') {
		status = gs_cgetent(buf, htype);
	}
	else {
		while (++ac < argc) {
			if (*av[ac] == '-') {
				if (av[ac][1] == 'h') {
					htype = av[ac + 1];
					status = gs_cgetent(buf, htype);
					break;
				}
			}
		}
	}

	if (status == GS_FAIL) {
		printf("Cannot open config file.\n");
		exit(1);
	}
	if (status == 0) {
		printf("Entry %s not found in config file\n", htype);
		exit(1);
	}
	printf("gs_cgetent(buf, \"%s\") returned %d\n\tbuf=%s\n",
		htype, status, buf);

	while ((c = getopt(argc, argv, "dh:s:f:b:n:")) != -1) {
		switch (c) {
		case 'd':
			debug = TRUE;
			break;

		case 'h':
			if (htype != NULL)
				break;
			htype = optarg;
			status = gs_cgetent(buf, htype);
			if (status == GS_FAIL) {
				printf("Cannot open config file.\n");
				exit(1);
			}
			if (status == 0) {
				printf("Entry %s not found in config file\n", htype);
				exit(1);
			}
			printf("gs_cgetent(buf, \"%s\") returned %d\nbuf=%s\n",
				htype, status, buf);
			break;

		case 'f':
		case 'b':
		/* test gs_cgetflag() */
			capability = optarg;
			status = gs_cgetflag(capability);
			printf("\tgs_cgetflag(%s) returned %d\n", capability, status);
			break;

		case 'n':
		/* test gs_cgetnum() */
			capability = optarg;
			status = gs_cgetnum(capability);
			printf("\tgs_cgetnum(%s) returned %d\n", capability, status);
			break;

		case 's':
		/* test gs_cgetstr() */
			capability = optarg;
			strptr = strbuf;
			outstr = gs_cgetstr(capability, &strptr);
			printf("\tgs_cgetstr(%s, 0x%lx = %s) returned '%s'\n",
				capability, &strptr, strptr, outstr);
			break;

		case '?':
			usage();
			exit(1);

		default:
			printf("Impossible argument parsing in getopt.i\n");
			exit(1);
		}
	}

	exit(0);
}

static void usage()
{
	printf("Usage:\n\t%s [-d -s string -f flag -n number]\n", av[0]);
}

#endif			     /* TEST */
