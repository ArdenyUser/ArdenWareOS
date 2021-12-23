-include ../config.mk

HG_SOURCES = hashgen.c 
HG_OBJECTS = ${HG_SOURCES:.c=.o} 
HG_OUT     = hashgen

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

all: ${HG_OUT}

${HG_OUT}: ${HG_OBJECTS}
	$(CC) $(LDFLAGS) -o ${HG_OUT} ${HG_SOURCES}


clean:
	rm ${HG_OBJECTS}
	rm ${HG_OUT}

