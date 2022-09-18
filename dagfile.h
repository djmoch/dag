/* See LICENSE file for copyright and license details */

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

int process_dagfile(struct dagfile *df, int verbose);
void free_dagfile(struct dagfile *df);
struct dagfile *parse_dagfile(FILE *path);
void debug_dagfile(struct dagfile *df);
