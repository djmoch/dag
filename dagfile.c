/* See LICENSE file for copyright and license details */
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "string.h"

#define HEADER "templates/header.html"
#define FOOTER "templates/footer.html"

enum {
	MD = 0,
	SCSS,
	HTML,
	CSS,
	PDF,
	OTHER
};

char *ext[] = { ".md", ".scss", "/index.html", ".css", ".pdf" };

char *xfrm[] = {
	NULL,
	NULL,
	"%s -Thtml %s >>%s",
	"%s -t compressed %s %s",
	"%s -Tms %s | /usr/local/bin/groff -ms -t -Tpdf >%s",
	"cp %s %s"
};

static int
getft(const char *infile)
{
	int ft = OTHER;

	for (int i=0; i<OTHER; i++) {
		if (strend(infile, ext[i])) {
			ft = i;
			break;
		}
	}

	return ft;
}

static char*
getout(const char *orig, const int from, const int to)
{
	int plen = strlen(orig) + strlen(ext[to]) - strlen(ext[from]);
	char *out = malloc(plen * sizeof(char));
	strcpy(out, orig);
	strnswp(out, ext[from], ext[to], plen);

	return out;
}

static char*
getcmd(int ft, const char *in, const char *out)
{
	char *ret, *cmd = xfrm[ft];
	int clen;

	switch (ft) {
	case CSS:
		clen = strlen(cmd) + strlen(in) + strlen(out) + strlen(SASSC) - 6;
		break;
	default:
		clen = strlen(cmd) + strlen(in) + strlen(out) + strlen(LOWDOWN) - 6;
	}

	ret = malloc(clen * sizeof(char));

	switch (ft) {
	case CSS:
		sprintf(ret, cmd, SASSC, in, out);
		break;
	case HTML:
	case PDF:
		sprintf(ret, cmd, LOWDOWN, in, out);
		break;
	default:
		sprintf(ret, cmd, in, out);
	}

	return ret;
}

static char*
gethdrcmd(const char * in, const char *out)
{
	char *ret, hdrpath[] = HEADER;
	const char *slug = out + strlen("target"); /* TODO - don't assume target */
	int clen;
	char cmd[] = "%s -DDESCRIPTION=\"$(%s -Xdescription -Tterm %s)\" -DPAGE_TITLE=\"$(%s -Xtitle -Tterm %s)\" -DSLUG=%s %s >%s";
	clen = strlen(cmd) + strlen(M4) + (2 * strlen(LOWDOWN)) + (2 * strlen(in)) +
		(2 * strlen(out)) + strlen(hdrpath) - 16;
	ret = malloc(clen);
	sprintf(ret, cmd, M4, LOWDOWN, in, LOWDOWN, in, slug, hdrpath, out);
	return ret;
}

static char*
getftrcmd(const char *out)
{
	char *ret, ftrpath[] = FOOTER;
	int clen;
	char cmd[] = "cat <%s >>%s";
	clen = strlen(cmd) + strlen(ftrpath) + strlen(out) - 6;
	ret = malloc(clen);
	sprintf(ret, cmd, ftrpath, out);
	return ret;
}

void
dag_mkdir(const char *out)
{
	struct stat sb;
	char fmt[] = "mkdir -p %s";
	int clen = strlen(fmt) + strlen(out) - 1;
	char cmd[clen];
	/* TODO: handle case where out exists, but isn't a directory */
	if (stat(out, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
		sprintf(cmd, fmt, out);
		printf("%s\n", cmd);
		system(cmd);
	}
}

int
outdated(char *dest, int nsrc, ...) {
	va_list ap;
	struct stat d_sb, s_sb;
	int rv = 1;

	if (stat(dest, &d_sb) != 0) {
		return rv;
	}

	va_start(ap, nsrc);
	for (int i=0; i<nsrc; i++) {
		stat(va_arg(ap, char *), &s_sb);
		if (s_sb.st_mtim.tv_sec > d_sb.st_mtim.tv_sec) {
			goto end;
		}
		else if ((s_sb.st_mtim.tv_sec == d_sb.st_mtim.tv_sec)
			&& (s_sb.st_mtim.tv_nsec > d_sb.st_mtim.tv_nsec)) {
			goto end;
		}
	}
	rv = 0;

end:
	va_end(ap);
	return rv;
}

int
process_dagfile(const char *infile, const char *outfile)
{
	char *out = NULL;
	char *cmd = NULL;

	switch (getft(infile)) {
	case MD:
		out = getout(outfile, MD, HTML);
		if (outdated(out, 3, HEADER, FOOTER, infile)) {
			dag_mkdir(dirname(out));
			cmd = gethdrcmd(infile, out);
			printf("%s\n", cmd);
			system(cmd);
			free(cmd);
			cmd = getcmd(HTML, infile, out);
			printf("%s\n", cmd);
			system(cmd);
			free(cmd);
			cmd = getftrcmd(out);
			printf("%s\n", cmd);
			system(cmd);
			free(out);
			free(cmd);
		}

		out = getout(outfile, MD, PDF);
		if (outdated(out, 3, HEADER, FOOTER, infile)) {
			dag_mkdir(dirname(out));
			cmd = getcmd(PDF, infile, out);
			printf("%s\n", cmd);
			system(cmd);
		}
		break;
	case SCSS:
		out = getout(outfile, SCSS, CSS);
		if (outdated(out, 1, infile)) {
			dag_mkdir(dirname(out));
			cmd = getcmd(CSS, infile, out);
			printf("%s\n", cmd);
			system(cmd);
		}
		break;
	default:
		out = malloc(strlen(outfile) * sizeof(char));
		strcpy(out, outfile);
		if (outdated(out, 1, infile)) {
			dag_mkdir(dirname(out));
			cmd = getcmd(OTHER, infile, outfile);
			printf("%s\n", cmd);
			system(cmd);
		}
	}

	free(out);
	if (cmd != NULL) {
		free(cmd);
	}

	return 0;
}
