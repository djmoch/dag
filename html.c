/* See LICENSE file for copyright and license details */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "db.h"
#include "string.h"

static const int DATE_STRING_LENGTH = 30; /* TODO: use decl in db.c */

static void htmlescape(char *s);

void
html_db_fmt(struct db_index *index)
{
	puts("<table>");
	struct db_entry *entry = index->entries;

	while (entry != NULL) {
		struct tm *pub_tm = NULL;
		char pub[DATE_STRING_LENGTH];

		if ((pub_tm = gmtime(&(entry->date_published))) == NULL) {
			errx(1, "call to gmtime failed");
		}
		if (strftime(pub, DATE_STRING_LENGTH, "%F", pub_tm) == 0) {
			errx(1, "call to strftime failed");
		}

		char *date = strdup(pub);
		char *title = strdup(entry->title);

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
