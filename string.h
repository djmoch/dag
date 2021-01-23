/* See LICENSE file for copyright and license details
 *
 * NOTE: All functions in this file assume strings are NUL-terminated
 * unless otherwise specified
 */

#ifndef __DAG_STRING_H
#define __DAG_STRING_H

int strbegin(const char *big, const char *little);
int strend(const char *big, const char *little);
char *strnswp(char *big, const char *old, const char *new, size_t max);
char *unquote(char *string);
char *strnesc(char *str, int maxlen);
int strcnt(char *str, char c);
int sprintf_ct(char *str, char *fmt, char *subst, int ct);

/* assumes the last element of strarray is NULL */
void dedup(char **strarray);

#endif
