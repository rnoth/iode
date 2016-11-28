CFLAGS    = -std=c89 -pedantic -Wall -Wextra -g -static
SRC       = main.c buffer.c
OBJ       = ${SRC:.c=.o}

MANPREFIX = $(PREFIX)

all: clean editor

.c.o:
	${CC} -c ${CFLAGS} $<

editor: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
	rm -f *.o

clean:
	rm -f editor ${OBJ}

install: editor
	mkdir -p  $(PREFIX)/bin $(MANPREFIX)/man/man1
	cp *.1 $(MANPREFIX)/man/man1/
	cp editor $(PREFIX)/bin/
