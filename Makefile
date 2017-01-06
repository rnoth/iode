CFLAGS    = -std=c89 -pedantic -Wall -Wextra -g -static
SRC       = main.c buffer.c utf8.c draw.c edit.c input.c actions.c
OBJ       = ${SRC:.c=.o}

MANPREFIX = $(PREFIX)

all: clean iode

.c.o:
	${CC} -c ${CFLAGS} $<

iode: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
	rm -f *.o

clean:
	rm -f iode ${OBJ}

install: iode
	mkdir -p  $(PREFIX)/bin $(MANPREFIX)/man/man1
	cp *.1 $(MANPREFIX)/man/man1/
	cp iode $(PREFIX)/bin/
