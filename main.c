#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "main.h"


int tty_fd = -1;


void
usage(void)
{
	fputs("Usage: iode [file]\n", stderr);
	exit(EXIT_FAILURE);
}


void
die(const char *message)
{
	perror(message);
	set_terminal();
	exit(EXIT_FAILURE);
}


void
set_option(char opt)
{
	extern int options[];
	options[tolower(opt)] = islower(opt);
}


/*
 * Switch terminal state between "normal" and "one char at a time".
 */
void
set_terminal()
{
	struct termios *termio = NULL;
	extern int tty_fd;

	if (tty_fd < 0) {
		if ((tty_fd = open("/dev/tty", O_RDWR)) < 0)
			die("open");
		termio->c_lflag &= ~(ICANON | ECHO | IGNBRK);
		tcsetattr(tty_fd, TCSANOW, termio);
	} else {
		termio->c_lflag |= ICANON | ECHO | IGNBRK;
		tcsetattr(tty_fd, TCSANOW, termio);
		if (close(tty_fd))
			die("close");
	}
}


int
main(int argc, char *argv[])
{
	extern FILE *tty_fp;
	extern Buffer *buffer;
	struct winsize winsize;

	int i, j, top = 0;
	char *filename = NULL;

	/* command line arguments */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] && argv[i][0] != '-' && argv[i][0] != '+') {
			filename = argv[i];

		} else if (isalpha(argv[i][1])) {
			for (j = 1; argv[i][j]; j++)
				set_option(argv[i][j]);

		} else if (isdigit(argv[i][1])) {
			for (top = 0, j = 1; argv[i][j]; j++) {
				if (argv[i][j] < '0' || argv[i][j] > '9')
					usage();
				top = 10 * top + argv[i][j] - '0';
			}
		}
	}

	/* buffer = buffer_read(filename); */

	set_terminal();

	if (ioctl(tty_fd, TIOCGWINSZ, &winsize) > 0)
		die("ioctl");

	/*
	draw_screen(buffer, winsize.ws_row, winsize.ws_col);

	for (; top > 1; top--)
		scroll_down(buffer, winsize.ws_row, winsize.ws_col);
	*/

	if (!(tty_fp = fopen("/dev/tty", "r")))
		die("fopen");
	/*
	input();
	*/

	/* resets the terminal to the previous state. */
	set_terminal();
	fprintf(stderr, "\033[%d;0H\033[?6c", winsize.ws_row);

	fclose(tty_fp);
	/*
	fclose(buffer->file);

	buffer_free(buffer);
	*/

	return EXIT_SUCCESS;
}
