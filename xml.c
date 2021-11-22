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

static char *xmlescape(char *s);
static void file_date_str(char mod[DATE_STRING_LENGTH], const char *path, const char *fmt);
static void time_t_date_str(char mod[DATE_STRING_LENGTH], time_t time, const char *fmt);

void
xml_db_fmt_sitemapindex(char *fqdn, struct db_index *index)
{
	char mod[DATE_STRING_LENGTH], *xfqdn;

	file_date_str(mod, index->db_path, "%FT%TZ");
	if ((xfqdn = strdup(fqdn)) == NULL) {
		err(errno, "strdup failed");
	}
	xfqdn = xmlescape(xfqdn);

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

	free(mod);
	free(xfqdn);
}

void
xml_db_fmt_sitemap(char *fqdn, struct db_index *index)
{
	struct db_entry *entry = index->entries;
	char *xfqdn;
	struct tm *tmp_tm;
	char upd[DATE_STRING_LENGTH], pub[DATE_STRING_LENGTH];

	if ((xfqdn = strdup(fqdn)) == NULL) {
		err(errno, "strdup failed");
	}
	xfqdn = xmlescape(xfqdn);

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
xml_db_fmt_rss(char *title, char *fqdn, char *description, char *rss_url,
		char *lang, char *copyright, struct db_index *index)
{
	struct db_entry *entry = index->entries;
	char *xcopyright, *xdescription, *xfqdn, *xlang, *xrss_url, *xtitle;
	char mod[DATE_STRING_LENGTH];
	const char date_fmt[] = "%a, %d %b %Y %T GMT";

	file_date_str(mod, index->db_path, date_fmt);

	if ((xcopyright = strdup(copyright)) == NULL) {
		err(errno, "strdup failed");
	}
	xcopyright = xmlescape(xcopyright);

	if ((xdescription = strdup(description)) == NULL) {
		err(errno, "strdup failed");
	}
	xdescription = xmlescape(xdescription);

	if ((xfqdn = strdup(fqdn)) == NULL) {
		err(errno, "strdup failed");
	}
	xfqdn = xmlescape(xfqdn);

	if ((xlang = strdup(lang)) == NULL) {
		err(errno, "strdup failed");
	}
	xlang = xmlescape(xlang);

	if ((xrss_url = strdup(rss_url)) == NULL) {
		err(errno, "strdup failed");
	}
	xrss_url = xmlescape(xrss_url);

	if ((xtitle = strdup(title)) == NULL) {
		err(errno, "strdup failed");
	}
	xtitle = xmlescape(xtitle);

	puts("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
	puts("<?xml-stylesheet type=\"text/xsl\" href=\"/assets/xml/rss.xsl\" media=\"all\"?>");
	puts("<rss version=\"2.0\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:atom=\"http://www.w3.org/2005/Atom\">");
	puts("  <channel>");
	printf("    <title>%s</title>\n", xtitle);
	printf("    <link>%s</link>\n", xfqdn);
	printf("    <description>%s</description>\n", xdescription);
	printf("    <atom:link href=\"%s\" rel=\"self\" type=\"application/rss+xml\"></atom:link>\n", xrss_url);
	printf("    <language>%s</language>\n", xlang);
	printf("    <copyright>%s</copyright>\n", xcopyright);
	printf("    <lastBuildDate>%s</lastBuildDate>\n", mod);
	puts("    <generator>Dag (git.danielmoch.com/dag)</generator>");
	puts("    <docs>http://blogs.law.harvard.edu/tech/rss</docs>");

	while (entry != NULL) {
		char *etitle, *elink, *ecreator, *edescription, *eslug;

		if ((etitle = strdup(entry->title)) == NULL) {
			err(errno, "strdup failed");
		}
		etitle = xmlescape(etitle);

		if ((elink = strdup(xfqdn)) == NULL) {
			err(errno, "strdup failed");
		}
		if ((elink = realloc(elink, strlen(xfqdn) + strlen(entry->slug) + 1)) == NULL) {
			err(errno, "realloc failed");
		}
		elink = strcat(elink, "/");
		elink = strcat(elink, entry->slug);
		elink = xmlescape(elink);

		if ((ecreator = strdup(entry->author)) == NULL) {
			err(errno, "strdup failed");
		}
		ecreator = xmlescape(ecreator);

		if ((edescription = strdup(entry->description)) == NULL) {
			err(errno, "strdup failed");
		}
		edescription = xmlescape(edescription);

		if ((eslug = strdup(entry->slug)) == NULL) {
			err(errno, "strdup failed");
		}
		eslug = xmlescape(eslug);

		time_t_date_str(mod, entry->date_published, date_fmt);

		puts("    <item>");
		printf("      <title>%s</title>\n", etitle);
		printf("      <link>%s</link>\n", elink);
		printf("      <dc:creator>%s</dc:creator>\n", ecreator);
		printf("      <description>%s</description>\n", edescription);
		printf("      <guid>%s</guid>\n", eslug);
		printf("      <pubDate>%s</pubDate>\n", mod);
		puts("    </item>");

		free(etitle);
		free(elink);
		free(ecreator);
		free(edescription);
		free(eslug);

		entry = entry->next;
	}

	puts("  </channel>");
	puts("</rss>");

	free(xcopyright);
	free(xdescription);
	free(xfqdn);
	free(xlang);
	free(xrss_url);
	free(xtitle);
}

static char *
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
		return s;
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
	strnswp(s, "\t", "&#x9;", maxlen);
	strnswp(s, "\n", "&#xA;", maxlen);
	strnswp(s, "\r", "&#xD;", maxlen);

	return s;
}

static void
file_date_str(char mod[DATE_STRING_LENGTH], const char *path, const char *fmt)
{
	struct stat sb;
	struct tm *time_tm;

	if (stat(path, &sb) == -1) {
		err(errno, "unable to stat %s", path);
	}
	if ((time_tm = gmtime(&(sb.st_mtim.tv_sec))) == NULL) {
		errx(1, "call to gmtime failed");
	}
	if (strftime(mod, DATE_STRING_LENGTH, fmt, time_tm) == 0) {
		errx(1, "call to strftime failed");
	}
}

static void
time_t_date_str(char mod[DATE_STRING_LENGTH], time_t time, const char *fmt)
{
	struct tm *time_tm;

	if ((time_tm = gmtime(&time)) == NULL) {
		errx(1, "call to gmtime failed");
	}
	if (strftime(mod, DATE_STRING_LENGTH, fmt, time_tm) == 0) {
		errx(1, "call to strftime failed");
	}
}
