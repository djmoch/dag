# See LICENSE file for copyright and license details
PREFIX := /usr/local
MANPATH := ${PREFIX}/share/man

SHDRS = dagfile.h \
	db.h \
	html.h \
	string.h \
	term.h \
	xml.h

HDRS = ${SHDRS} \
	y.tab.h

DSRC = dag.c \
	dagfile.c \
	string.c \
	y.tab.c \
	lex.yy.c

ISRC = dagindex.c \
	db.c \
	html.c \
	string.c \
	term.c \
	xml.c

DIST_SRC = dag.c \
	dagfile.c \
	string.c \
	dagindex.c \
	db.c \
	html.c \
	string.c \
	string_test.c \
	term.c \
	xml.c \
	dag.1 \
	dagindex.1 \
	dagfile.5 \
	Makefile \
	README \
	config.mk \
	parse.y \
	parse.l \
	${SHDRS}

DOBJ = ${DSRC:.c=.o}
IOBJ = ${ISRC:.c=.o}
LIBS = -ll -ly
VERSION = 0.1.0dev0

CPPFLAGS := -DVERSION=\"${VERSION}\"
CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O0 -g -c -pipe
LDFLAGS := ${LIBS}
