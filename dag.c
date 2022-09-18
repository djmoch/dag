/* See LICENSE file for copyright and license details */
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dagfile.h"
#include "string.h"

static char *argv0;

enum {
	ERR_NONE,
	ERR_DAGFILE,
	ERR_UNKNOWN_OPTION,
	ERR_ARGS,
	ERR_FILE,
	ERR_PARSE
};

static void
usage(int rv)
{
	FILE *fp;

	if (rv) {
		fp = stderr;
	}
	else {
		fp = stdout;
	}

	fprintf(fp, "usage: %s [-Vhv] [-f file]\n", argv0);
	exit(rv);
}

int
main(int argc, char **argv)
{
	char ch, *path = "Dagfile";
	int verbose = 0;
	FILE *df;
	argv0 = basename(argv[0]);
	struct dagfile *dagfile;
	int retval;

	while ((ch = getopt(argc, argv, "f:hvV")) != -1) {
		switch (ch) {
		case 'f':
			path = optarg;
			break;
		case 'h':
			usage(ERR_NONE);
			break;
		case 'v':
			verbose += 1;
			break;
		case 'V':
			printf("%s %s\n", argv0, VERSION);
			exit(ERR_NONE);
			break;
		default:
			usage(ERR_UNKNOWN_OPTION);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		fprintf(stderr, "%s: expected no arguments\n", argv0);
		usage(ERR_ARGS);
	}
#ifdef __OpenBSD__
	if (pledge("stdio cpath rpath wpath proc exec", NULL)) {
		err(errno, "pledge failed");
	}
#endif
	if ((df = fopen(path, "r")) == NULL) {
		errx(ERR_FILE, "%s: error opening %s", argv0, path);
	}
	if ((dagfile = parse_dagfile(df)) == NULL) {
		errx(ERR_PARSE, "%s: error parsing %s", argv0, path);
	}
	fclose(df);
	if (verbose) {
		debug_dagfile(dagfile);
	}

	retval = process_dagfile(dagfile, verbose);

	free_dagfile(dagfile);

	return retval;
}
