/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
make_m4_cmd(const char **defines, const int num_defines,
		const char *infile, const char *outfile) {
	char *retval = malloc(strlen(M4) + 1);
	sprintf(retval, M4);

	for (int i=0; i<num_defines; i++) {
		/* "-D" + defines[i] + " " + '\0' */
		int len = strlen(defines[i]) + 4;

		retval = realloc(retval, strlen(retval) + len);
		sprintf(retval, "%s -D%s", retval, defines[i]);
	}

	retval = realloc(retval, strlen(retval) + strlen(infile) +
		strlen(" > ") + strlen(outfile));
	sprintf(retval, "%s %s > %s", retval, infile, outfile);

	return retval;
}
