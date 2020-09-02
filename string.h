/* See LICENSE file for copyright and license details
 *
 * NOTE: All functions in this file assume strings are NUL-terminated
 * unless otherwise specified
 */

#ifndef __DAG_STRING_H
#define __DAG_STRING_H

int strbegin(const char *big, const char *little);
char *strnswp(char *big, const char *old, const char *new, size_t max);

/* assumes the last element of strarray is NULL */
void dedup(char **strarray);

#endif
