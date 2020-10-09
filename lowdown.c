/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

static const char redir[] = " > ";

char *
lowdown_html_cmd(const char *infile, const char *outfile)
{
	char *retval = malloc(strlen(LOWDOWN) + 1);
	sprintf(retval, LOWDOWN);

	const char html[] = " -Thtml ";

	retval = realloc(retval, strlen(retval) + strlen(html) +
		strlen(infile) + strlen(redir) + strlen(outfile));
	sprintf(retval, "%s %s > %s", retval, infile, outfile);

	return retval;
}

char *
lowdown_pdf_cmd(const char *infile, const char *outfile)
{
	char *retval = malloc(strlen(LOWDOWN) + 1);
	sprintf(retval, LOWDOWN);

	const char pdf[] = " -Tms | TROFF |
	retval = realloc(retval, strlen(retval) + strlen(infile) +
		strlen(redir) + strlen(outfile));
	sprintf(retval, "%s %s > %s", retval, infile, outfile);

	return retval;
}
