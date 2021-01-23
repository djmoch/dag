%{
/* See LICENSE file for copyright and license details */
#include <stdio.h>
#include <string.h>
#include "dagfile.h"
#include "string.h"

extern FILE *yyin;

static struct dagfile *dagfile;
static struct target *t;
static struct source *so;
static struct extension *e;
static struct suffix *s;
%}

%union {
	char *str;
}

%token <str> TARGET SOURCE EXTENSION REQUIRE FILTER SUFFIX NONE STRING

%%

targets: target targets
	| target
	;

target: target_name "{" sources "}"
	;

target_name: TARGET STRING	{ push_target($2); }
	;

sources: source sources
	| source
	;

source: source_name "{" extensions "}"
	| source_name
	;

source_name: SOURCE STRING { push_source($2); }
	;

extensions: extension extensions
	| extension
	;

extension: extension_name "{" suffixes "}"
	;

extension_name: EXTENSION STRING	{ push_extension($2); }
	;

suffixes: suffix suffixes
	| suffix
	;

suffix: suffix_name "{" requirements filters "}"
	| suffix_name "{" filters "}"
	;

suffix_name: SUFFIX STRING	{ push_suffix($2); }
	;

requirements: requirement requirements
	| requirement
	;

requirement: REQUIRE STRING { push_requirement($2); }
	;

filters: filter filters
	| filter
	;

filter: FILTER STRING { push_filter($2); }
	;

%%

int
parse_dagfile(struct dagfile *df, FILE *file)
{
	dagfile = df;
	dagfile->target = NULL;
	t = NULL;
	e = NULL;
	s = NULL;
	yyin = file;
	return yyparse();
}

void
push_target(char *string)
{
	if (t != NULL) {
		printf("error: only one target block allowed\n");
		exit(1);
	}

	t = malloc(sizeof(struct target));
	t->path = strdup(unquote(string));
	t->sources = NULL;
	dagfile->target = t;
}

void
push_source(char *string)
{
	so = malloc(sizeof(struct source));
	so->path = strdup(unquote(string));
	so->extensions = NULL;
	so->next = NULL;

	if (t->sources == NULL) {
		t->sources = so;
		return;
	}

	struct source *ts = t->sources;

	while (ts->next != NULL) {
		ts = ts->next;
	}
	ts->next = so;
}

void
push_extension(char *string)
{
	e = malloc(sizeof(struct extension));
	e->value = strdup(unquote(string));
	e->suffixes = NULL;
	e->next = NULL;

	if (so->extensions == NULL) {
		so->extensions = e;
		return;
	}

	struct extension *se = so->extensions;

	while (se->next != NULL) {
		se = se->next;
	}
	se->next = e;
}

void
push_suffix(char *string)
{
	s = malloc(sizeof(struct suffix));
	s->value = strdup(unquote(string));
	s->requirements = NULL;
	s->filters = NULL;
	s->next = NULL;

	if (e->suffixes == NULL) {
		e->suffixes = s;
		return;
	}

	struct suffix *es = e->suffixes;

	while (es->next != NULL) {
		es = es->next;
	}
	es->next = s;
}

void
push_requirement(char *string)
{
	struct requirement *r = malloc(sizeof(struct requirement));
	r->path = strdup(unquote(string));
	r->next = NULL;

	if (s->requirements == NULL) {
		s->requirements = r;
		return;
	}

	struct requirement *sr = s->requirements;

	while (sr->next != NULL) {
		sr = sr->next;
	}
	sr->next = r;
}

void
push_filter(char *string)
{
	struct filter *f = malloc(sizeof(struct filter));
	f->cmd = strdup(unquote(string));
	f->next = NULL;

	if (s->filters == NULL) {
		s->filters = f;
		return;
	}

	struct filter *sf = s->filters;

	while (sf->next != NULL) {
		sf = sf->next;
	}
	sf->next = f;
}

void
debug_dagfile(struct dagfile *df)
{
	printf("target %s\n", df->target->path);
	struct source *s = df->target->sources;
	while (s != NULL) {
		printf("source %s\n", s->path);
		struct extension *e = s->extensions;
		while (e != NULL) {
			printf("extension %s\n", e->value);
			struct suffix *s = e->suffixes;
			while (s != NULL) {
				printf("suffix %s\n", s->value);
				struct requirement *r = s->requirements;
				while (r != NULL) {
					printf("require %s\n", r->path);
					r = r->next;
				}
				struct filter *f = s->filters;
				while (f != NULL) {
					printf("filter %s\n", f->cmd);
					f = f->next;
				}
				s = s->next;
			}
			e = e->next;
		}
		s = s->next;
	}
}