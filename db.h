/* See LICENSE file for copyright and license details */
#ifndef __DB_H
#define __DB_H

struct db_entry {
	char *author;
	char *category;
	char *description;
	char *date_published;
	char *slug;
	char *title;
	char *date_updated;
	struct db_entry *next;
};

struct db_index {
	const char *db_path;
	struct db_entry *entries;
};

struct db_index *db_index_open(const char *db_file);
void db_index_write(struct db_index *index);
void db_index_close(struct db_index *index);
void db_entry_add(struct db_index *index, struct db_entry *entry);
void db_entry_fprint(FILE *fp, struct db_entry *entry);

#endif
