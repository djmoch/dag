# See LICENSE file for copyright and license details

LOWDOWN := /usr/local/bin/lowdown
SASSC := /usr/local/bin/sassc
M4 := /usr/bin/m4
TBL := /usr/local/bin/tbl

PREFIX := /usr/local
MANPATH := ${PREFIX}/share/man
X11BASE := /usr/X11R6

HDRS = dagfile.h string.h y.tab.h
DSRC = dag.c dagfile.c string.c y.tab.c lex.yy.c
ISRC = dagindex.c string.c
DIST_SRC = ${SRC} Makefile README config.mk
DOBJ = ${DSRC:.c=.o}
IOBJ = ${ISRC:.c=.o}
LIBS = -ll -ly
VERSION = 0.1.0dev0

CPPFLAGS := -DVERSION=\"${VERSION}\"
CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O0 -g -c -pipe
LDFLAGS := ${LIBS}
