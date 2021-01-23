/* See LICENSE file for copyright and license details */

#ifndef __DAGFILE_H
#define __DAGFILE_H

struct requirement {
	char *path;
	struct requirement *next;
};

struct filter {
	char *cmd;
	struct filter *next;
};

struct suffix {
	char *value;
	struct requirement *requirements;
	struct filter *filters;
	struct suffix *next;
};

struct extension {
	char *value;
	struct suffix *suffixes;
	struct extension *next;
};

struct source {
	char *path;
	struct extension *extensions;
	struct source *next;
};

struct target {
	char *path;
	struct source *sources;
};

struct dagfile {
	struct target *target;
};

int process_dagfile(struct dagfile *df);

#endif
