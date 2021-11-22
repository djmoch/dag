# See LICENSE file for copyright and license details
.POSIX:

include config.mk

all: dag dagindex

y.tab.o: y.tab.c y.tab.h
	${CC} -Wno-implicit-function-declaration -c -o y.tab.o y.tab.c

lex.yy.o: lex.yy.c y.tab.h
	${CC} -c -o lex.yy.o lex.yy.c

.c.o: ${HDRS}
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ $<

y.tab.h y.tab.c: parse.y
	yacc -d parse.y

lex.yy.c: parse.l
	lex parse.l

dag: ${DOBJ}
	${CC} -o $@ ${DOBJ} ${LDFLAGS}

dagindex: ${IOBJ}
	${CC} -o $@ ${IOBJ}

string_test: string_test.o string.o
	${CC} -o $@ string_test.o string.o

install: dag dagindex
	install -Dm755 dag ${DESTDIR}${PREFIX}/bin/dag
	install -Dm755 dagindex ${DESTDIR}${PREFIX}/bin/dagindex

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dag
	rm -f ${DESTDIR}${PREFIX}/bin/dagindex

clean:
	rm -f *.o dag dagindex *_test y.tab.* lex.yy.c

test: string_test
	./string_test

dist:
	rm -rf dag-${VERSION}
	mkdir dag-${VERSION}
	cp ${DIST_SRC} dag-${VERSION}
	tar -cf dag-${VERSION} | gzip > dag-${VERSION}.tar.gz

distclean:
	rm -rf dag-${VERSION}
	rm -f dag-${VERSION}.tar.gz

clean-all: clean distclean

.PHONY: all clean install uninstall dist distclean clean-all test
