# See LICENSE file for copyright and license details
.POSIX:

include config.mk

all: dag

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ $<

dag: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

string_test: string_test.o string.o
	${CC} -o $@ string_test.o string.o ${LDFLAGS}

install: dag
	install -Dm755 dag ${DESTDIR}${PREFIX}/bin/dag

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dag

clean:
	rm -f *.o dag *_test

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

.PHONY: all clean install uninstall dist distclean clean-all
