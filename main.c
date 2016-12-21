#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "main.h"


void
usage(void)
{
	fputs("Usage: iode [file] -[" OPTIONS "]\n", stderr);
	exit(EXIT_FAILURE);
}


void
die(const char *message)
{
	perror(message);
	unraw_terminal(0);
	exit(EXIT_FAILURE);
}


int
set_option(char opt)
{
	extern int options[];
	options[tolower(opt)] = islower(opt);

	return !!strchr(OPTIONS, opt);
}


void
raw_terminal(int fd)
{
	struct termios termios_p;

	if (tcgetattr(fd, &termios_p))
		die("tcgetattr");

	termios_p.c_lflag &= ~(ICANON | ECHO | IGNBRK);

	if (tcsetattr(fd, TCSANOW, &termios_p))
		die("tcsetattr");
}


void
unraw_terminal(int fd)
{
	struct termios termios_p;

	if (tcgetattr(fd, &termios_p))
		die("tcgetattr");

	termios_p.c_lflag |= (ICANON | ECHO | IGNBRK);

	if (tcsetattr(fd, TCSANOW, &termios_p))
		perror("tcsetattr");
}


int
main(int argc, char *argv[])
{
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
				if (!set_option(argv[i][j]))
					usage();

		} else if (isdigit(argv[i][1])) {
			for (top = 0, j = 1; argv[i][j]; j++) {
				if (argv[i][j] < '0' || argv[i][j] > '9')
					usage();
				top = 10 * top + argv[i][j] - '0';
			}
		}
	}

	/* buffer = buffer_read(filename); */

	raw_terminal(0);

	if (ioctl(0, TIOCGWINSZ, &winsize) > 0)
		die("ioctl");

	/*
	draw_screen(buffer, winsize.ws_row, winsize.ws_col);

	for (; top > 1; top--)
		scroll_down(buffer, winsize.ws_row, winsize.ws_col);

	input();
	*/

	/* resets the terminal to the previous state. */
	unraw_terminal(0);

	/*
	fclose(buffer->file);

	buffer_free(buffer);
	*/

	return EXIT_SUCCESS;
}
