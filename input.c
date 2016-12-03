#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Perform action associated with key
 */
int
input_key(FILE *tty_fp, int tty_fd, Buffer *buffer)
{
	struct winsize w;

	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	switch (fgetc(tty_fp)) {

	case CONTROL('C'):
		return EXIT_FAILURE;

	case 'j':
		scroll_down(buffer, w.ws_row, w.ws_col);
		break;

	case 'k':
		scroll_up(buffer, w.ws_row, w.ws_col);
		break;
	}
		return CONTINUE;
}


/*
 * Listen for the user input and call the appropriate functions.
 */
int
input_get(FILE *tty_fp, int tty_fd, Buffer *buffer)
{
	int exit_code;

	/* main execution loop: get input char by char from the keyboard */
	while ((exit_code = input_key(tty_fp, tty_fd, buffer)) == CONTINUE);

	return exit_code;
}
