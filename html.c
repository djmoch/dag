/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "string.h"

static void htmlescape(char *s);

void
html_db_fmt(struct db_index *index)
{
	puts("<table>");
	struct db_entry *entry = index->entries;

	while (entry != NULL) {
		char *date = strdup(entry->date_published);
		char *title = strdup(entry->title);

		date[10] = '\0'; /* only interested in YYYY-MM-DD */

		htmlescape(date);
		htmlescape(title);

		printf("<tr><td><time>%s</time></td>", date);
		printf("<td><a href=\"/%s\">%s</a></td></tr>\n", entry->slug, title);
		entry = entry->next;

		free(date);
		free(title);
	}
	puts("</table>");
}

static void
htmlescape(char *s)
{
	size_t len = strlen(s), ct = 0, maxlen = 0;

	for (size_t i=0; i<len; i++) {
		switch (s[i]) {
		case '"':
		case '\'':
		case '&':
		case '<':
		case '>':
			ct += 1;
			break;
		}
	}

	if (ct == 0) {
		return;
	}

	maxlen = len + (ct * strlen("&quot;"));
	s = realloc(s, maxlen);

	strnswp(s, "'", "&#39;", maxlen);
	strnswp(s, "\"", "&quot;", maxlen);
	strnswp(s, "&", "&amp;", maxlen);
	strnswp(s, "<", "&lt;", maxlen);
	strnswp(s, ">", "&gt;", maxlen);
}
