/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <time.h>

#include "db.h"

void
term_db_fmt(struct db_index *index)
{
	int i = 1;
	struct db_entry *entry = index->entries;

	while (entry != NULL) {
		printf("Entry %d\n", i);
		db_entry_fprint(stdout, entry);
		entry = entry->next;
		i += 1;
	}
}
