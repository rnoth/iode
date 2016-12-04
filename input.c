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
	char            k;
	struct winsize  w;
	Arg            *a         = malloc(sizeof(Arg));

	k_pager[CONTROL('L')] = a_redraw;
	k_pager[CONTROL('c')] = a_quit;
	k_pager['q']          = a_quit;
	k_pager['g']          = a_jump_begin;
	k_pager['G']          = a_jump_end;
	k_pager['u']          = a_half_page_up;
	k_pager['d']          = a_half_page_down;
	k_pager['f']          = a_page_up;
	k_pager['b']          = a_page_down;
	k_pager['k']          = a_scroll_up;
	k_pager['j']          = a_scroll_down;

	/* main execution loop: get input char by char from the keyboard */
	while (exit_code != EXIT_FAILURE && exit_code != EXIT_SUCCESS) {

		if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
			die("ioctl");

		a->c = w.ws_col;
		a->r = w.ws_row;
		a->b = buffer;

		k = fgetc(tty_fp);

		/* multipliers for commands */
		while (k >= '0' && k <= '9' && a->m < 1000) {
			a->m = a->m * 10 + (k - '0');
			sprintf(a->b->operators, "%d", a->m ? a->m : 0);
			update_status_line(a->b, a->r, a->c);
			fgetc(tty_fp);
		}

		exit_code = k_pager[(int) k] ? k_pager[(int) k](a) : CONTINUE;
	}

	free(a);

	return exit_code;
}
