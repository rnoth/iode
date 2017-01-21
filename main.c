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
	fputs("Usage: iode [+-][" FLAGS "] [file]\n", stderr);
	exit(EXIT_FAILURE);
}


enum { RAW, RESET };  /* mode */

/*
 * Set terminal to raw mode or reset it back.
 *
 * RAW opens /dev/tty in tty_fd and RESET it
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


void
die(const char *message)
{
	extern struct line * l_first;

	perror(message);

	if (l_first)
		free_buffer(l_first);
	fclose(file);
	set_terminal(RESET);

	exit(EXIT_FAILURE);
}


int
set_flag(char flag, int value)
{
	extern int flags[];

	flags[tolower(flag)] = value;

	return strchr(FLAGS, tolower(flag)) != NULL;
}


int
main(int argc, char *argv[])
{
	extern size_t n_top;

	int i, j;
	char *filename = NULL;

	n_top = 1;

	/* command line arguments */
	for (i = 1; i < argc; i++) {
		if (!argv[i][0]) {
			usage();

		} else if (argv[i][0] != '-' && argv[i][0] != '+') {
			filename = argv[i];

		} else if (isdigit(argv[i][1])) {
			for (n_top = 1, j = 1; argv[i][j]; j++) {
				if (argv[i][j] < '0' || argv[i][j] > '9')
					usage();
				n_top = 10 * n_top + (unsigned) argv[i][j] - '0';
			}

		} else {
			for (j = 1; argv[i][j]; j++) {
				if (!set_flag(argv[i][j], argv[i][0] == '+')) {
					usage();
				}
			}
		}
	}

	read_buffer(filename);

	set_terminal(RAW);

	/* save screen state */
	fputs("\033[s\033[?1049h", stderr);

	update_terminal_size();
	draw_screen();

	get_input();

	/* restore screen state */
	fputs("\033[u\033[?1049l", stderr);

	set_terminal(RESET);

	fclose(file);
	free_buffer(l_first);

	return EXIT_SUCCESS;
}
