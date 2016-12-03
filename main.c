#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Set terminal to send one char at a time for interactive mode, and return the
 * last terminal state.
 */
struct termios
set_terminal(int tty_fd)
{
	struct termios termio_old;
	struct termios termio_new;

	/* set the terminal to send one key at a time. */

	/* get the terminal's state */
	if (tcgetattr(tty_fd, &termio_old) < 0)
		die("Can not get terminal attributes with tcgetattr().");

	/* create a new modified state by switching the binary flags */
	termio_new          = termio_old;
	termio_new.c_lflag &= ~(ICANON | ECHO | IGNBRK);

	/* apply this state to current terminal now (TCSANOW) */
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
main(int argc, char *arcgv[])
{
	int             tty_fd     = open("/dev/tty", O_RDWR);
	FILE           *tty_fp     = fopen("/dev/tty", "r");
	struct termios  termio_old = set_terminal(tty_fd);
	struct winsize  w;
	Buffer         *buffer     = file_read(stdin);

	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	draw_screen(buffer, w.ws_row, w.ws_col);

	input_get(tty_fp);

	/* resets the terminal to the previous state. */
	tcsetattr(tty_fd, TCSANOW, &termio_old);
	fclose(tty_fp);

	buffer_free(buffer);

	return EXIT_SUCCESS;
}
