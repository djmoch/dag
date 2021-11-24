# See LICENSE file for copyright and license details
PREFIX := /usr/local
MANPATH := ${PREFIX}/share/man

HDRS = dagfile.h string.h y.tab.h
DSRC = dag.c dagfile.c string.c y.tab.c lex.yy.c
ISRC = dagindex.c db.c html.c string.c term.c xml.c
DIST_SRC = ${SRC} Makefile README config.mk
DOBJ = ${DSRC:.c=.o}
IOBJ = ${ISRC:.c=.o}
LIBS = -ll -ly
VERSION = 0.1.0dev0

CPPFLAGS := -DVERSION=\"${VERSION}\"
CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O0 -g -c -pipe
LDFLAGS := ${LIBS}
