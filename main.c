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
	fputs("Usage: iode [file]\n", stderr);
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
	int             code, i, j, top = 0;
	int             tty_fd   = open("/dev/tty", O_RDWR);
	FILE           *tty_fp   = fopen("/dev/tty", "r");
	char           *filename = NULL;
	Buffer         *buffer   = NULL;
	struct termios  termio;
	struct winsize  w;

	/* command line arguments */
	for (i = 1; i < argc; i++) {

		if (argv[i][0] == '-') {
			switch (argv[i][1]) {

			case 'R':

				break;

			default:
				usage();
				break;
			}

		} else if (argv[i][0] == '+') {
			for (top = 0, j = 1; argv[i][j]; j++) {
				if (argv[i][j] < '0' || argv[i][j] > '9')
					usage();
				top = 10 * top + argv[i][j] - '0';
			}

		} else {
			filename = argv[i];
		}
	}

	buffer = buffer_read(filename);

	termio = set_terminal(tty_fd);
	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	fprintf(stderr, "\033[2J\033[%d;0H\033[?16c", w.ws_row);
	draw_screen(buffer, w.ws_row, w.ws_col);

	/* jump to initial position */
	for (; top > 1; top--)
		scroll_down(buffer, w.ws_row, w.ws_col);

	code = input(tty_fp, tty_fd, buffer);

	/* resets the terminal to the previous state. */
	tcsetattr(tty_fd, TCSANOW, &termio);
	fclose(tty_fp);
	fclose(buffer->file);
	fprintf(stderr, "\033[%d;0H\033[?6c", w.ws_row);

	buffer_free(buffer);

	return code;
}
