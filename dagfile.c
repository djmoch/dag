/* See LICENSE file for copyright and license details */
#include <libgen.h>
#include <stdarg.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fts.h>

#include "dagfile.h"
#include "string.h"

static void process_source(char *file);
static void process_extension(char *file);
static void process_suffix(char *file);
static void copy_file(char *file);
static char *make_outpath(const char *file);
static int outdated(char *dest, int nsrc, ...);
static void dag_mkdir(const char *out);
static char *fmt_filter(char *f_cmd, char *file, char *target);
static char *expand(char *cmd, char pat, char *subst);

static struct target *tgt;
static struct source *src;
static struct extension *ext;
static struct suffix *sfx;

static int verbose = 0;

int
process_dagfile(struct dagfile *df, int verb)
{
	FTSENT *entry = NULL;
	FTS *src_tree;
	tgt = df->target;
	src = df ->target->sources;
	char *path[] = { src->path, NULL };
	ext = NULL;
	sfx = NULL;

	verbose = verb;

	if (verbose >= 2) {
		printf("process_dagfile\n");
	}

	while (src != NULL && path[0] != NULL) {
		src_tree = fts_open(path, FTS_LOGICAL, NULL);
		if (src_tree == NULL) {
			err(errno, "call to fts_open() failed:");
		}

		while ((entry = fts_read(src_tree)) != NULL) {
			if ((entry->fts_info & FTS_F) == FTS_F) {
				process_source(entry->fts_accpath);
			}
		}

		if (errno != 0) {
			err(errno, "error occurred walking file tree:");
		}

		fts_close(src_tree);
		src = src->next;
		if (src != NULL) {
			path[0] = src->path;
		}
	}
	return 0;
}

void
free_dagfile(struct dagfile *df)
{
	struct source *s = df->target->sources;
	while (s != NULL) {
		struct source *snext = s->next;
		struct extension *e = s->extensions;
		while (e != NULL) {
			struct extension *enext = e->next;
			struct suffix *sfx = e->suffixes;
			while (sfx != NULL) {
				struct suffix *sfxnext = sfx->next;
				struct requirement *r = sfx->requirements;
				struct filter *f = sfx->filters;
				while (r != NULL) {
					struct requirement *rnext = r->next;
					free(r->path);
					free(r);
					r = rnext;
				}
				while (f != NULL) {
					struct filter *fnext = f->next;
					free(f->cmd);
					free(f);
					f = fnext;
				}
				free(sfx->value);
				free(sfx);
				sfx = sfxnext;
			}
			free(e->value);
			free(e);
			e = enext;
		}
		free(s->path);
		free(s);
		s = snext;
	}
	free(df->target->path);
	free(df->target);
	free(df);
}

static void
process_source(char *file)
{
	int found = 0;
	ext = src->extensions;

	if (verbose >= 2) {
		printf("process_source(%s)\n", file);
	}

	while (ext != NULL) {
		if (strend(file, ext->value)) {
			process_extension(file);
			found = 1;
		}
		ext = ext->next;
	}

	if (!found) {
		/* no extension matches, so just copy */
		copy_file(file);
	}
}

static void
process_extension(char *file)
{
	sfx = ext->suffixes;

	if (verbose >= 2) {
		printf("process_extension(%s, %s)\n", file, ext->value);
	}

	while (sfx != NULL) {
		process_suffix(file);
		sfx = sfx->next;
	}
}

static void
process_suffix(char *file)
{
	char *target = make_outpath(file);
	struct requirement *r = sfx->requirements;

	if (verbose >= 2) {
		printf("process_suffix(%s, %s)\n", file, sfx->value);
	}

	int old = outdated(target, 1, file);
	while (r != NULL) {
		old |= outdated(target, 1, r->path);
		r = r->next;
	}

	if (old) {
		if (verbose) {
			printf("%s outdated, applying filters\n", file);
		}
		struct filter *f = sfx->filters;
		while (f != NULL) {
			char *cmd = fmt_filter(f->cmd, file, target);
			printf("%s\n", cmd);
			system(cmd);
			free(cmd);
			f = f->next;
		}
	}
	else if (verbose) {
		printf("%s up to date\n", file);
	}
	free(target);
}

static void
copy_file(char *file)
{
	char *target = make_outpath(file);

	if (outdated(target, 1, file)) {
		if (verbose) {
			printf("%s outdated, applying filters\n", file);
		}
		char fmt[] = "cp %s %s";
		char *cmd;

		if ((cmd = malloc((strlen(file) + strlen(target) + strlen(fmt) + 1) * sizeof(char))) == NULL) {
			err(errno, "malloc failed");
		}

		sprintf(cmd, fmt, file, target);
		printf("%s\n", cmd);
		system(cmd);
		free(cmd);
	}
	else if (verbose) {
		printf("%s up to date\n", file);
	}
	free(target);
}

static char *
make_outpath(const char *file)
{
	int len = strlen(file) + strlen(tgt->path) + 1;
	char *target;

	if ((target = malloc(len * sizeof(char))) == NULL) {
		err(errno, "malloc failed");
	}

	strcpy(target, file);
	if (verbose >= 2) {
		printf("make_outpath: target = %s\n", target);
	}
	strnswp(target, src->path, tgt->path, len);
	if (verbose >= 2) {
		printf("make_outpath: target = %s\n", target);
	}

	/*
	 * TODO: find a better way to determine if the suffix needs
	 * to be changed
	 */
	if (sfx) {
		char *tmp;
		len = strlen(target) + strlen(sfx->value) + 1;
		if ((tmp = malloc(len * sizeof(char))) == NULL) {
			err(errno, "malloc failed");
		}
		strcpy(tmp, target);
		if (verbose >= 2) {
			printf("make_outpath: tmp = %s\n", target);
		}
		strnswp(tmp, ext->value, sfx->value, len);
		if (verbose >= 2) {
			printf("make_outpath: tmp = %s\n", target);
		}
		free(target);
		target = tmp;
		if (verbose >= 2) {
			printf("make_outpath: target = %s\n", target);
		}
	}

	dag_mkdir(dirname(target));
	if (verbose >= 2) {
		printf("make_outpath(%s) = %s\n", file, target);
	}
	return target;
}

static int
outdated(char *dest, int nsrc, ...)
{
	va_list ap;
	struct stat d_sb, s_sb;
	int rv = 1;
	int r;

	if ((r = stat(dest, &d_sb)) != 0) {
		if (verbose) {
			printf("stat(%s): %d\n", dest, r);
		}
		return rv;
	}
	else {
		if (verbose) {
			printf("stat(%s): %d\n", dest, r);
		}
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

static void
dag_mkdir(const char *out)
{
	struct stat sb;
	char fmt[] = "mkdir -p %s";
	int clen = strlen(fmt) + strlen(out) + 1;
	char cmd[clen];
	/* TODO: handle case where out exists, but isn't a directory */
	if (stat(out, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
		sprintf(cmd, fmt, out);
		printf("%s\n", cmd);
		system(cmd);
	}
}

static char *
fmt_filter(char *f_cmd, char *file, char *target)
{
	struct requirement *r = sfx->requirements;
	char i = '1';
	char *cmd;

	if ((cmd = strdup(f_cmd)) == NULL) {
		err(errno, "strdup failed");
	}

	while (r != NULL) {
		if (i > '9')
			err(1, "too many requirements: %s\n", f_cmd);

		cmd = expand(cmd, i, r->path);
		r = r->next;
		i += 1;
	}
	cmd = expand(cmd, '<', file);
	cmd = expand(cmd, '>', target);
	return cmd;
}

static char *
expand(char *cmd, char pat, char *subst)
{
	char *tmp;
	int ct = 0;
	int len = strlen(cmd) + strcnt(cmd, '%') + 1;
	if ((cmd = realloc(cmd, len)) == NULL) {
		err(errno, "realloc failed");
	}
	strnesc(cmd, len);
	for (int j=0;;j++) {
		if (cmd[j] == '\0')
			break;
		if (ct >= 10)
			err(1, "too many substitutions\n");

		if (cmd[j] == '$' && cmd[j+1] == pat) {
			ct += 1;
			cmd[j] = '%';
			cmd[j+1] = 's';
		}
	}
	if ((tmp = malloc((strlen(cmd) + (ct * strlen(subst)) + 1) * sizeof(char))) == NULL) {
		err(errno, "malloc failed");
	}
	sprintf_ct(tmp, cmd, subst, ct);
	free(cmd);
	return tmp;
}
