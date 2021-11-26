/* See LICENSE file for copyright and license details */

void xml_db_fmt_sitemapindex(char *fqdn, struct db_index *index);
void xml_db_fmt_sitemap(char *fqdn, struct db_index *index);
void xml_db_fmt_rss(char *title, char *fqdn, char *description,
	char *rss_url, char *lang, char *copyright, struct db_index *index);
