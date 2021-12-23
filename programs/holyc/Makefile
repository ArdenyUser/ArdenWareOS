-include config.mk

SOURCES         = src/main.c src/lexer.c src/util.c src/args.c src/decipher.c src/parser.c
OBJECTS         = ${SOURCES:.c=.o}
TARGET          = holyc
INSTALL_DIR     = /usr/bin
VERSION         = 0.0.17

all: tools ${TARGET}

.c.o:
	$(CC) ${CFLAGS} -DHOLYC_BUILD_VERSION='"${VERSION}"' -c $< -o $@

${TARGET}: ${OBJECTS} 
	$(CC) ${LDFLAGS} -o $@ ${OBJECTS} ${LDLIBS}

tools:
	make -C tools all
	./tools/hashgen src/defs.h

clean:
	make -C tools clean
	rm src/*.o
	rm src/defs.h
	rm ${TARGET}

install: all
	mkdir -p ${INSTALL_DIR}/
	install -s -m755 ${TARGET} ${INSTALL_DIR}

uninstall:
	$(RM) ${INSTALL_DIR}/${TARGET}

version:
	@echo ${VERSION}

info:
	@echo "CC      -> $(CC)"
	@echo "AR      -> $(AR)"
	@echo "CFLAGS  -> ${CFLAGS}"
	@echo "LDFLAGS -> ${LDFLAGS}"
	@echo "LDLIBS  -> ${LDLIBS}"

.PHONY: clean install uninstall version build_info tools
