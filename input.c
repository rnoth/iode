#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "main.h"
#include "keybindings.h"


/*
 * Listen for the user input and call the appropriate functions.
 *
 * Main execution loop.
 */
int
input(FILE *tty_fp, int tty_fd, Buffer *buffer)
{
	int             exit_code = CONTINUE;
	char            key;
	struct winsize  w;
	Arg            *a         = malloc(sizeof(Arg));

	/* main execution loop: get input char by char from the keyboard */
	while (exit_code != EXIT_FAILURE && exit_code != EXIT_SUCCESS) {

		if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
			die("ioctl");

		a->c = w.ws_col;
		a->r = w.ws_row;

		key = fgetc(tty_fp);

		/* multipliers for commands */
		while (key >= '0' && key <= '9' && a->m < 1000) {
			a->m = a->m * 10 + (key - '0');
			sprintf(a->b->operators, "%d", a->m ? a->m : 0);
			update_status_line(a->b, a->r, a->c);
			fgetc(tty_fd);
		}

		exit_code = k_pager[key] ? k_pager[key]() : CONTINUE;
	}

	free(a);

	return exit_code;
}
