#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Apply a modifier to the argument, to pass to commands.
 */
char
input_multiplier(Arg *a, char k, FILE *tty_fp)
{
	a->m = 0;

	while (k >= '0' && k <= '9' && a->m < 1000) {
		a->m = a->m * 10 + (k - '0');
		sprintf(a->b->operators, "%d", a->m);
		update_status_line(a->b, a->r, a->c);
		k = fgetc(tty_fp);
	}

	a->b->operators[0] = '\0';
	update_status_line(a->b, a->r, a->c);

	return k;
}


/*
 * Listen for the user input and call the appropriate functions.
 *
 * Main execution loop.
 */
int
input(FILE *tty_fp, int tty_fd, Buffer *buffer)
{
	int mode = CONTINUE;
	char k;
	struct winsize w;
	Arg *a = malloc(sizeof(Arg));
	int (*keys[10][128])(Arg *);

#include "keybindings.h"

	a->b = buffer;

	/* main execution loop: get input char by char from the keyboard */
	while (buffer->mode != EXIT_FAILURE && buffer->mode != EXIT_SUCCESS) {

		k = fgetc(tty_fp);

		if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
			die("ioctl");
		a->c = w.ws_col;
		a->r = w.ws_row;

		if (buffer->mode == PAGER)
			k = input_multiplier(a, k, tty_fp);

		/* execute action associated with key */
		if (keys[buffer->mode][(int) k])
			mode = keys[buffer->mode][(int) k](a);

		/* rules to switch modes and submodes */
		mode = ((mode == CONTINUE) ? buffer->mode : mode);

		buffer->mode = mode;
	}

	free(a);

	return buffer->mode;
}
