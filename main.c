#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>

#include "main.h"


void
usage(void)
{
	fputs("Usage: triode [-] [file]\n", stderr);
	exit(EXIT_FAILURE);
}


/*
 * Set terminal to send one char at a time for interactive mode, and return the
 * last terminal state.
 */
struct termios
set_terminal(int tty_fd)
{
	struct termios termio_old, termio_new;

	if (tcgetattr(tty_fd, &termio_old) < 0)
		die("Can not get terminal attributes with tcgetattr().");

	termio_new          = termio_old;
	termio_new.c_lflag &= ~(ICANON | ECHO | IGNBRK);

	tcsetattr(tty_fd, TCSANOW, &termio_new);

	return termio_old;
}


void
die(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}


int
main(int argc, char *argv[])
{
	int             code, i;
	int             tty_fd   = open("/dev/tty", O_RDWR);
	FILE           *tty_fp   = fopen("/dev/tty", "r");
	FILE           *file     = NULL;
	char           *filename = NULL;
	Buffer         *buffer   = NULL;
	struct termios  termio;
	struct winsize  w;

	/* command line arguments */
	for (i = 1; i < argc; i++) {

		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case '\0':
				file = stdin;
				break;
			default:
				usage();
				break;
			}

		} else if (filename || file) {
			fputs(PROGRAM_NAME": can only open one file\n", stderr);
			exit(EXIT_FAILURE);
		} else {
			filename = argv[i];
		}
	}

	if (!filename && !file) {
		fputs(PROGRAM_NAME": no file to read\n", stderr);
		exit(EXIT_FAILURE);

	} else if (!file && !(file = fopen(filename, "r"))) {
		die("fopen");
	}

	buffer = buffer_read(file);

	termio = set_terminal(tty_fd);
	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	fputs("\033[13;0H\033[?16c", stderr);
	draw_screen(buffer, w.ws_row, w.ws_col);

	code = input(tty_fp, tty_fd, buffer);

	/* resets the terminal to the previous state. */
	tcsetattr(tty_fd, TCSANOW, &termio);
	fclose(tty_fp);
	fclose(file);
	fprintf(stderr, "\033[%d;0H\033[?6c", w.ws_row);

	buffer_free(buffer);

	return code;
}
