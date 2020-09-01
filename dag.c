/* See LICENSE file for copyright and license details */
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <unistd.h>

#include "string.h"

static char *argv0;

enum {
	ERR_NONE,
	ERR_UNKNOWN_OPTION,
	ERR_ARGS,
	ERR_FTS
};

void
usage(int e_val) {
	FILE *fp;

	if (e_val) {
		fp = stderr;
	}
	else {
		fp = stdout;
	}

	fprintf(fp, "usage: %s [-hv] [-Dname=value ...] [-H header] [-f footer] in ... out\n", argv0);
	exit(e_val);
}

char *
make_m4_cmd(const char **defines, const int num_defines, const char *infile, const char *outfile) {
	char *retval = malloc(strlen(M4) + 1);
	sprintf(retval, M4);

	for (int i=0; i<num_defines; i++) {
		/* "-D" + defines[i] + " " + '\0' */
		int len = strlen(defines[i]) + 4;

		retval = realloc(retval, strlen(retval) + len);
		sprintf(retval, "%s -D%s", retval, defines[i]);
	}

	retval = realloc(retval, strlen(retval) + strlen(infile) + strlen(" > ") +
		strlen(outfile));
	sprintf(retval, "%s %s > %s", retval, infile, outfile);

	return retval;
}

char *
make_outpath(const char *out, const char *srcpath, char *src_argv[]) {
	for (int i=0; src_argv[i]!=NULL; i++) {
		if (strbegin(srcpath, src_argv[i])) {
			char *outpath = NULL;
			size_t plen = 0;
			if (strlen(src_argv[i]) >= strlen(out))
				plen = strlen(srcpath);
			else
				plen = strlen(srcpath) + strlen(out) - strlen(src_argv[i]);
			outpath = malloc(plen * sizeof(char));
			strcpy(outpath, srcpath);
			strnswp(outpath, src_argv[i], out, plen);
			return outpath;
		}
	}

	return NULL;
}

int
main(int argc, char **argv) {
	char ch, *header = NULL, *footer = NULL, *defines[MAX_DEFINES];
	char *out;
	int num_defines = 0;
	argv0 = basename(argv[0]);
	FTSENT *entry = NULL;

	while ((ch = getopt(argc, argv, "D:H:f:vh")) != -1) {
		switch (ch) {
		case 'D':
			defines[num_defines] = optarg;
			num_defines += 1;
			break;
		case 'f':
			footer = optarg;
			break;
		case 'H':
			header = optarg;
			break;
		case 'h':
			usage(ERR_NONE);
			break;
		case 'v':
			printf("%s %s\n", argv0, VERSION);
			exit(ERR_NONE);
			break;
		default:
			usage(ERR_UNKNOWN_OPTION);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 2) {
		fprintf(stderr, "%s: missing input and/or output directory\n", argv0);
		usage(ERR_ARGS);
	}

	char *path_argv[argc];
	for (int i=0; i<argc - 1; i++) {
		path_argv[i] = argv[i];
	}
	path_argv[argc - 1] = NULL;
	out = argv[argc - 1];

	FTS *src_tree = fts_open(path_argv, FTS_LOGICAL, NULL);
	if (errno != 0) {
		fprintf(stderr, "%s: call to fts_open() failed -- %d\n",
			argv0, errno);
	}

	while ((entry = fts_read(src_tree)) != NULL) {
		if ((entry->fts_info & FTS_F) == FTS_F) {
			char *outpath = make_outpath(out, entry->fts_accpath, path_argv);
			printf("%s: found file -- %s\n",
				argv0, entry->fts_accpath);
			printf("%s: creating file -- %s/%s\n", argv0, out, entry->fts_accpath);
			free(outpath);
		}
	}

	if (errno != 0) {
		fprintf(stderr, "%s: error occurred walking file tree -- %d\n",
			argv0, errno);
		exit(ERR_FTS);
	}

	/* find .md files */
	/* process if newer than corresponding html file in output */
	/* also need to process if any template file is newer */
}
