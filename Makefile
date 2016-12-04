CFLAGS    = -std=c89 -pedantic -Wall -Wextra -g -static
SRC       = main.c buffer.c draw.c input.c
OBJ       = ${SRC:.c=.o}

MANPREFIX = $(PREFIX)

all: clean triode

.c.o:
	${CC} -c ${CFLAGS} $<

triode: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
	rm -f *.o

clean:
	rm -f triode ${OBJ}

install: triode
	mkdir -p  $(PREFIX)/bin $(MANPREFIX)/man/man1
	cp *.1 $(MANPREFIX)/man/man1/
	cp triode $(PREFIX)/bin/
