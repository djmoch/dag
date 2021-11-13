/* See LICENSE file for copyright and license details */
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "db.h"
#include "string.h"

static const int DATE_STRING_LENGTH = 30; /* TODO: use decl in db.c */

static void xmlescape(char *s);

void
xml_db_fmt_sitemapindex(char *fqdn, struct db_index *index)
{
	struct stat sb;
	struct tm *time_tm;
	char mod[DATE_STRING_LENGTH], *xfqdn = strdup(fqdn);

	if (stat(index->db_path, &sb) == -1) {
		err(errno, "unable to stat %s", index->db_path);
	}
	if ((time_tm = gmtime(&(sb.st_mtim.tv_sec))) == NULL) {
		errx(1, "call to gmtime failed");
	}
	if (strftime(mod, DATE_STRING_LENGTH, "%FT%TZ", time_tm) == 0) {
		errx(1, "call to strftime failed");
	}

	xmlescape(mod);
	xmlescape(xfqdn);

	puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	puts("<sitemapindex");
	puts("    xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"");
	puts("    xmlns:xhtml=\"http://www.w3.org/1999/xhtml\"");
	puts("    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
	puts("    xsi:schemaLocation=\"http://www.sitemaps.org/schemas/sitemap/0.9");
	puts("                        http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd\">");
	puts("<sitemap>");
	printf("	<loc>%s/sitemap.xml</loc>\n", xfqdn);
	printf("	<lastmod>%s</lastmod>\n", mod);
	puts("</sitemap>");
	puts("</sitemapindex>");

	free(xfqdn);
}

void
xml_db_fmt_sitemap(char *fqdn, struct db_index *index)
{
	struct db_entry *entry = index->entries;
	char *xfqdn = strdup(fqdn);
	struct tm *tmp_tm;
	char upd[DATE_STRING_LENGTH], pub[DATE_STRING_LENGTH];

	xmlescape(xfqdn);

	puts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	puts("<urlset");
	puts("    xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"");
	puts("    xmlns:xhtml=\"http://www.w3.org/1999/xhtml\"");
	puts("    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
	puts("    xsi:schemaLocation=\"http://www.sitemaps.org/schemas/sitemap/0.9");
	puts("                        http://www.sitemaps.org/schemas/sitemap/0.9/sitemap.xsd\">");
	while (entry != NULL) {
		if ((tmp_tm = gmtime(&(entry->date_published))) == NULL) {
			errx(1, "call to gmtime failed");
		}
		if (strftime(pub,  DATE_STRING_LENGTH, "%FT%TZ", tmp_tm) == 0) {
			errx(1, "call to strftime failed");
		}
		if ((tmp_tm = gmtime(&(entry->date_updated))) == NULL) {
			errx(1, "call to gmtime failed");
		}
		if (strftime(upd,  DATE_STRING_LENGTH, "%FT%TZ", tmp_tm) == 0) {
			errx(1, "call to strftime failed");
		}
		puts("<url>");
		printf("	<loc>%s/%s</loc>\n", xfqdn, entry->slug);
		if (entry->date_updated) {
			printf("	<lastmod>%s</lastmod>\n", upd);
		}
		else {
			printf("	<lastmod>%s</lastmod>\n", pub);
		}
		puts("</url>");
		entry = entry->next;
	}
	puts("</urlset>");

	free(xfqdn);
}

void
xml_db_fmt_rss(struct db_index *index)
{
	index = NULL;
}

static void
xmlescape(char *s)
{
	size_t len = strlen(s), ct = 0, maxlen = 0;

	for (size_t i=0; i<len; i++) {
		switch (s[i]) {
		case '"':
		case '\'':
		case '&':
		case '<':
		case '>':
		case '\t':
		case '\n':
		case '\r':
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
	strnswp(s, "\t", "&#x9;", maxlen);
	strnswp(s, "\n", "&#xA;", maxlen);
	strnswp(s, "\r", "&#xD;", maxlen);
}
