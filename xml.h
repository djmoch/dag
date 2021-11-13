/* See LICENSE file for copyright and license details */
#ifndef __XML_H
#define __XML_H

void xml_db_fmt_sitemapindex(char *fqdn, struct db_index *index);
void xml_db_fmt_sitemap(char *fqdn, struct db_index *index);
void xml_db_fmt_rss(char *fqdn, struct db_index *index);

#endif
