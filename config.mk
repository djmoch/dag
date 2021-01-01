# See LICENSE file for copyright and license details

LOWDOWN := /usr/local/bin/lowdown
SASSC := /usr/local/bin/sassc
M4 := /usr/bin/m4

PREFIX := /usr/local
MANPATH := ${PREFIX}/share/man
X11BASE := /usr/X11R6

SRC = dag.c dagfile.c string.c
DIST_SRC = ${SRC} Makefile README config.mk
OBJ = ${SRC:.c=.o}
INCS =
LIBS =
VERSION = 0.1.0dev0

CPPFLAGS := -DVERSION=\"${VERSION}\" -DLOWDOWN=\"${LOWDOWN}\" -DSASSC=\"${SASSC}\" -DM4=\"${M4}\"
CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O1 -c ${INCS} -pipe
LDFLAGS := ${LIBS}
