#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "main.h"


int tty_fd;


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
	set_terminal(RESET);
	exit(EXIT_FAILURE);
}


int
set_option(char opt)
{
	extern int options[];
	options[tolower(opt)] = islower(opt);

	return !!strchr(OPTIONS, opt);
}


/*
 * Set terminal to raw mode or reset it back.
 *
 * @mode:
 * - RAW (opening /dev/tty in tty_fd);
 * - RESET (closing  tty_fd).
 */
void
set_terminal(int mode)
{
	extern int tty_fd;

	struct termios termios_p;

	if (mode == RAW)
		tty_fd = open("/dev/tty", O_RDWR);

	if (tcgetattr(tty_fd, &termios_p)) {
		perror("tcgetattr");
		exit(EXIT_FAILURE);
	}

	if (mode == RAW) {
		termios_p.c_lflag &= ~(ICANON | ECHO | IGNBRK);
	} else if (mode == RESET) {
		termios_p.c_lflag |=  (ICANON | ECHO | IGNBRK);
	}

	if (tcsetattr(tty_fd, TCSANOW, &termios_p)) {
		perror("tcsetattr");
		exit(EXIT_FAILURE);
	}

	if (mode == RESET)
		tty_fd = close(tty_fd);
}


int
main(int argc, char *argv[])
{
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

	read_buffer(filename);

	set_terminal(RAW);

	update_terminal_size();
	draw_screen();
	/*
	for (; top > 1; top--)
		scroll_down(buffer, w.ws_row, w.ws_col);

	input();
	*/

	/* resets the terminal to the previous state. */
	set_terminal(RESET);

	fclose(file);
	free_buffer(l_first);

	return EXIT_SUCCESS;
}
