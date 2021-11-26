/* See LICENSE file for copyright and license details */
#include <err.h>
#include <errno.h>
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
	char *date, *title;
	puts("<table>");
	struct db_entry *entry = index->entries;
	struct tm *pub_tm = NULL;
	char pub[DATE_STRING_LENGTH];

	while (entry != NULL) {
		if (entry->sitemap_only) {
			entry = entry->next;
			continue;
		}
		if ((pub_tm = gmtime(&(entry->date_published))) == NULL) {
			errx(1, "call to gmtime failed");
		}
		if (strftime(pub, DATE_STRING_LENGTH, "%F", pub_tm) == 0) {
			errx(1, "call to strftime failed");
		}

		if ((date = strdup(pub)) == NULL) {
			err(errno, "strdup failed");
		}

		if ((title = strdup(entry->title)) == NULL) {
			err(errno, "strdup failed");
		}

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
	if ((s = realloc(s, maxlen)) == NULL) {
		err(errno, "realloc failed");
	}

	strnswp(s, "'", "&#39;", maxlen);
	strnswp(s, "\"", "&quot;", maxlen);
	strnswp(s, "&", "&amp;", maxlen);
	strnswp(s, "<", "&lt;", maxlen);
	strnswp(s, ">", "&gt;", maxlen);
}
