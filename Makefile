CFLAGS    = -std=c89 -pedantic -Wall -Wextra -g -static #-Wconversion
SRC       = main.c buffer.c actions.c draw.c edit.c utf8.c input.c
OBJ       = ${SRC:.c=.o}
NAME      = iode

MANPREFIX = $(PREFIX)

all: clean ${NAME}

.c.o:
	${CC} -c ${CFLAGS} $<

${NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}
	rm -f *.o

clean:
	rm -f ${NAME} ${OBJ}

install: ${NAME}
	mkdir -p  $(PREFIX)/bin $(MANPREFIX)/man/man1
	cp *.1 $(MANPREFIX)/man/man1/
	cp ${NAME} $(PREFIX)/bin/
