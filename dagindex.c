/* See LICENSE file for copyright and license details */
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "db.h"
#include "html.h"
#include "string.h"
#include "term.h"
#include "xml.h"

enum {
	ERR_NONE,
	ERR_UNKNOWN_OPTION,
	ERR_ARGS,
	ERR_TIME
};

enum {
	NONE,
	ADD,
	GENERATE
};

static const char index_file[] = "index.db";
static char *argv0;
static int mode = NONE;
int verbose = 0;

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

	fprintf(fp, "usage: %s -Vh\n", argv0);
	fprintf(fp, "\t%s -A -t title -s slug -p date_published [-a author]\n", argv0);
	fprintf(fp, "\t\t[-u date_updated] [-c category] [-d description]\n");
	fprintf(fp, "\t%s -G -o fmt [-f fqdn]\n", argv0);
	exit(rv);
}

static char *
populate_str(const char *src, const char *dft)
{
	char *dst;

	if (src) {
		if (verbose >= 2) {
			fprintf(stderr, "DEBUG: populate_field src is non-NULL: %s\n", src);
		}
		dst = strdup(src);
	}
	else {
		if (verbose >= 2) {
			fprintf(stderr, "DEBUG: populate_field src is NULL\n");
		}
		dst = strdup(dft);
	}

	if (dst == NULL) {
		err(errno, "strdup failed in populate_str");
	}

	return dst;
}

/*
 * Takes a string formatted like 2020-09-03 15:56:17 UTC-04:00
 * and returns a time_t
 */
static time_t
populate_time(char *time_str)
{
	struct tm time_tm;
	time_t time = 0;

	if (time_str) {
		if (strptime(time_str, "%F %T UTC%z",  &time_tm) == NULL) {
			err(ERR_TIME, "call to strptime failed");
		}

		if ((time = mktime(&time_tm)) == -1) {
			err(ERR_TIME, "call to mktime failed");
		}
	}

	return time;
}

int
main(int argc, char **argv)
{
	struct db_entry *entry;
	struct db_index *index;
        char ch, *author = NULL, *cat = NULL, *desc = NULL,
		*fmt = NULL, *fqdn = NULL, *pub = NULL, *slug = NULL,
		*title = NULL, *updated = NULL;

	argv0 = basename(argv[0]);

	while ((ch = getopt(argc, argv, "AGVa:c:d:f:ho:p:s:t:u:v")) != -1) {
		switch (ch) {
		case 'A':
			if (mode != NONE) {
				warnx("specifying -A here makes no sense");
				usage(ERR_ARGS);
			}
			mode = ADD;
			entry = malloc(sizeof(struct db_entry));
			break;
		case 'G':
			if (mode != NONE) {
				warnx("specifying -G here makes no sense");
				usage(ERR_ARGS);
			}
			mode = GENERATE;
			break;
		case 'V':
			printf("%s %s\n", argv0, VERSION);
			exit(ERR_NONE);
			break;
		case 'a':
			if (mode != ADD) {
				warnx("specifying -a without -A makes no sense");
				usage(ERR_ARGS);
			}
			author = optarg;
			break;
		case 'c':
			if (mode != ADD) {
				warnx("specifying -c without -A makes no sense");
				usage(ERR_ARGS);
			}
			cat = optarg;
			break;
		case 'd':
			if (mode != ADD) {
				warnx("specifying -d without -A makes no sense");
				usage(ERR_ARGS);
			}
			desc = optarg;
			break;
		case 'f':
			if (mode != GENERATE) {
				warnx("specifying -f without -G makes no sense");
				usage(ERR_ARGS);
			}
			fqdn = optarg;
			break;
		case 'h':
			usage(ERR_NONE);
			break;
		case 'o':
			if (mode != GENERATE) {
				warnx("specifying -o without -G makes no sense");
				usage(ERR_ARGS);
			}
			fmt = optarg;
			break;
		case 'p':
			if (mode != ADD) {
				warnx("specifying -p without -A makes no sense");
				usage(ERR_ARGS);
			}
			pub = optarg;
			break;
		case 's':
			if (mode != ADD) {
				warnx("specifying -s without -A makes no sense");
				usage(ERR_ARGS);
			}
			slug = optarg;
			break;
		case 't':
			if (mode != ADD) {
				warnx("specifying -t without -A makes no sense");
				usage(ERR_ARGS);
			}
			title = optarg;
			break;
		case 'u':
			if (mode != ADD) {
				warnx("specifying -u without -A makes no sense");
				usage(ERR_ARGS);
			}
			updated = optarg;
			break;
		case 'v':
			verbose += 1;
			break;
		default:
			usage(ERR_UNKNOWN_OPTION);
		}
	}
	argc -= optind;
	argv += optind;

	if (argc > 0) {
		warnx("received %d arguments, but expected none", argc);
		usage(ERR_ARGS);
	}
	if (mode == ADD && title == NULL) {
		warnx("must specify -t with -A");
		usage(ERR_ARGS);
	}
	if (mode == ADD && slug == NULL) {
		warnx("must specify -s with -A");
		usage(ERR_ARGS);
	}
	if (mode == ADD && pub == NULL) {
		warnx("must specify -p with -A");
		usage(ERR_ARGS);
	}

#ifdef __OpenBSD__
	if (unveil("index.db", "rwc")) {
		err(errno, "unveil failed");
	}
	if (pledge("stdio rpath wpath cpath", "")) {
		err(errno, "pledge failed");
	}
#endif

	if (verbose >= 2) {
		fputs("DEBUG: received options:\n", stderr);
		fprintf(stderr, "\tauthor = %s\n", author);
		fprintf(stderr, "\tcategory = %s\n", cat);
		fprintf(stderr, "\tdescription = %s\n", desc);
		fprintf(stderr, "\tdate_published = %s\n", pub);
		fprintf(stderr, "\tslug = %s\n", slug);
		fprintf(stderr, "\ttitle = %s\n", title);
		fprintf(stderr, "\tdate_updated = %s\n", updated);
	}

	index = db_index_open(index_file);

	switch (mode) {
	case ADD:
		entry->author = populate_str(author, "");
		entry->category = populate_str(cat, "");
		entry->description = populate_str(desc, "");
		entry->date_published = populate_time(pub);
		entry->slug = populate_str(slug, "");
		entry->title = populate_str(title, "");
		entry->date_updated = populate_time(updated);
		entry->next = NULL;

		db_entry_add(index, entry);
		db_index_write(index);
		break;
	case GENERATE:
		if (strncmp(fmt, "term", 5) == 0) {
			term_db_fmt(index);
		}
		else if (strncmp(fmt, "html", 5) == 0) {
			html_db_fmt(index);
		}
		else if (strncmp(fmt, "sitemapindex", 13) == 0) {
			if (fqdn == NULL) {
				warnx("sitemapindex requires fqdn be set with -f");
				break;
			}
			xml_db_fmt_sitemapindex(fqdn, index);
		}
		else if (strncmp(fmt, "sitemap", 8) == 0) {
			if (fqdn == NULL) {
				warnx("sitemap requires fqdn be set with -f");
				break;
			}
			xml_db_fmt_sitemap(fqdn, index);
		}
		else {
			warnx("unknown output format: %s", fmt);
		}
		break;
	default:
		warnx("unexpected mode: %d", mode);
		db_index_close(index);
		usage(ERR_ARGS);
	}

	db_index_close(index);
	return 0;
}
