/* See LICENSE file for copyright and license details */
#include <err.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "db.h"

static int DATE_STRING_LENGTH = 30;
extern int verbose;

static struct db_entry *getentry(FILE *fp);
static void free_entry(struct db_entry *entry);
static char *getstring(FILE *fp);
static int entrycmp(struct db_entry *e1, struct db_entry *e2);

struct db_index *
db_index_open(const char *db_path)
{
	char *dir, *path = malloc(strlen(db_path + 1));
	struct stat sb;
	struct db_index *index =  malloc(sizeof(struct db_index));
	struct db_entry *cur_entry = NULL, *next_entry = NULL;
	FILE *fp = fopen(db_path, "rb");

	strcpy(path, db_path);
	dir = dirname(path);
	index->db_path = db_path;
	index->entries = NULL;

	if (stat(dir, &sb)) {
		err(errno, "failed to stat parent directory %s", dir);
	}
	if (stat(db_path, &sb)) {
		warnx("creating new db file: %s", db_path);
		index->db_path = db_path;
		index->entries = NULL;
		goto exit;
	}

	if (fp == NULL) {
		err(errno, "failed to open %s for reading", db_path);
	}

	if ((cur_entry = getentry(fp)) == NULL) {
		warnx("empty db file: %s", db_path);
		goto exit;
	}

	index->entries = cur_entry;

	while ((next_entry = getentry(fp)) != NULL) {
		cur_entry->next = next_entry;
		cur_entry = next_entry;
	}

exit:
	if (fp != NULL) {
		fclose(fp);
	}
	free(path);
	return index;
}

void
db_index_write(struct db_index *index)
{
	FILE *fp = fopen(index->db_path, "wb");
	const char nul = '\0';
	struct db_entry *entry = index->entries;

	if (fp == NULL) {
		err(errno, "failed to open %s for writing", index->db_path);
	}

	while (entry != NULL) {
		if (verbose) {
			warnx("writing entry:");
			db_entry_fprint(stderr, entry);
		}
		fputs(entry->author, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->category, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->description, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->date_published, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->slug, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->title, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		fputs(entry->date_updated, fp);
		fwrite(&nul, sizeof(char), 1, fp);
		entry = entry->next;
	}

	fclose(fp);
}

void
db_index_close(struct db_index *index)
{
	struct db_entry *entry = index->entries,
		*next_entry = NULL;

	while (entry != NULL) {
		if (verbose >= 2) {
			fprintf(stderr, "DEBUG: freeing index entry\n");
		}
		next_entry = entry->next;
		free_entry(entry);
		entry = next_entry;
	}

	free(index);
}

void
db_entry_add(struct db_index *index, struct db_entry *entry)
{
	struct db_entry *cur_entry = index->entries,
		*prev_entry = NULL;

	if (verbose) {
		warnx("adding entry:");
		db_entry_fprint(stderr, entry);
	}

	if (cur_entry == NULL) {
		if (verbose) {
			warnx("writing empty index");
		}
		index->entries = entry;
		return;
	}

	while (cur_entry != NULL) {
                int dcmp = strncmp(entry->date_published,
			cur_entry->date_published, DATE_STRING_LENGTH);
		int scmp = strcmp(entry->slug, cur_entry->slug);
		if (verbose >= 2) {
			fprintf(stderr, "DEBUG: dcmp = %d, scmp = %d\n", dcmp, scmp);
		}
		if (dcmp > 0) {
			if (verbose) {
				warnx("inserting new entry %s", entry->slug);
			}
			entry->next = cur_entry;
			if (prev_entry != NULL) {
				prev_entry->next = entry;
			}
			else {
				index->entries = entry;
			}
			break;
		}
		else if ((dcmp == 0) && (scmp == 0)) {
			if (!entrycmp(cur_entry, entry)) {
				warnx("new entry %s matches existing one", entry->slug);
				break;
			}

			if (verbose) {
				warnx("updating existing entry %s", entry->slug);
			}
			if (prev_entry != NULL) {
				prev_entry->next = entry;
			}
			else {
				index->entries = entry;
			}
			entry->next = cur_entry->next;
			free_entry(cur_entry);
			break;
		}
		else if (cur_entry->next == NULL) {
			if (verbose) {
				warnx("inserting new entry %s at end of list", entry->slug);
			}
			cur_entry->next = entry;
			break;
		}

		prev_entry = cur_entry;
		cur_entry = cur_entry->next;
	}
}

void
db_entry_fprint(FILE *fp, struct db_entry *entry)
{
	fprintf(fp, "\tauthor = %s\n", entry->author);
	fprintf(fp, "\tcategory = %s\n", entry->category);
	fprintf(fp, "\tdescription = %s\n", entry->description);
	fprintf(fp, "\tdate_published = %s\n", entry->date_published);
	fprintf(fp, "\tslug = %s\n", entry->slug);
	fprintf(fp, "\ttitle = %s\n", entry->title);
	fprintf(fp, "\tdate_updated = %s\n", entry->date_updated);
}

static struct db_entry *
getentry(FILE *fp)
{
	struct db_entry *entry = NULL;

	/*
	 * EOF is only permissible at the entry boundary
	 */
	char * str = getstring(fp);
	if (str == NULL) {
		return NULL;
	}

	entry = malloc(sizeof(struct db_entry));
	entry->author         = str;

	if ((entry->category = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	if((entry->description = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	if((entry->date_published = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	if((entry->slug = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	if((entry->title = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	if((entry->date_updated = getstring(fp)) == NULL) {
		errx(1, "index db file corrupt!");
	}
	entry->next = NULL;

	return entry;
}

static void
free_entry(struct db_entry *entry)
{
	free(entry->author);
	free(entry->category);
	free(entry->description);
	free(entry->date_published);
	free(entry->slug);
	free(entry->title);
	free(entry->date_updated);
	free(entry);
}

static char *
getstring(FILE * fp)
{
	char *str = NULL;
	size_t bufsize = 0;

	if (getdelim(&str, &bufsize, '\0', fp) == -1) {
		return NULL;
	}

	return str;
}

static int
entrycmp(struct db_entry *e1, struct db_entry *e2)
{
	int cmp;

	if ((cmp = strcmp(e1->author, e2->author)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->category, e2->category)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->description, e2->description)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->date_published, e2->date_published)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->slug, e2->slug)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->title, e2->title)) != 0) {
		return cmp;
	}
	if ((cmp = strcmp(e1->date_updated, e2->date_updated)) != 0) {
		return cmp;
	}

	return 0;
}
