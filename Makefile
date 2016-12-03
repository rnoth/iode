CFLAGS    = -std=c89 -pedantic -Wall -Wextra -g -static
SRC       = main.c buffer.c draw.c input.c
OBJ       = ${SRC:.c=.o}

MANPREFIX = $(PREFIX)

all: clean pager

.c.o:
	${CC} -c ${CFLAGS} $<

pager: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
	rm -f *.o

clean:
	rm -f pager ${OBJ}

install: pager
	mkdir -p  $(PREFIX)/bin $(MANPREFIX)/man/man1
	cp *.1 $(MANPREFIX)/man/man1/
	cp pager $(PREFIX)/bin/
