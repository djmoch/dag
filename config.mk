# See LICENSE file for copyright and license details

# Paths to helper tools
GROFF = \"/usr/local/bin/groff\"
LOWDOWN = \"/usr/local/bin/lowdown\"
M4 = \"/usr/bin/m4\"

# Number of DEFINES to allow from the command line
MAX_DEFINES = 100

PREFIX := /usr/local
MANPATH := ${PREFIX}/share/man
X11BASE := /usr/X11R6

SRC = dag.c string.c m4.c lowdown.c
DIST_SRC = ${SRC} Makefile README config.mk
OBJ = ${SRC:.c=.o}
INCS =
LIBS =
VERSION = 0.1.0dev0

CPPFLAGS := -DVERSION=\"${VERSION}\" -DGROFF=${GROFF} \
	-DLOWDOWN=${LOWDOWN} -DM4=${M4} -DMAX_DEFINES=${MAX_DEFINES}
CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O1 -c ${INCS} -pipe
#CFLAGS := -std=c99 -pedantic-errors -Wall -Wextra -Werror -O0 -g -c ${INCS} -pipe
LDFLAGS := ${LIBS}
