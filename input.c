#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "keybindings.h"


/*
 * Apply a modifier to the argument, to pass to commands.
 */
void
input_multiplier(char k, FILE *tty_fp)
{
	extern int multiplier;
	extern char operators[];

	while (k >= '0' && k <= '9' && multiplier < 1000) {
		multiplier = multiplier * 10 + (k - '0');
		sprintf(operators, "%d", multiplier);
		update_status_line();
		k = fgetc(tty_fp);
	}

	operators[0] = '\0';
	update_status_line();
}


/*
 * Listen for the user input and call the appropriate functions.
 *
 * Main execution loop.
 */
int
input(FILE *tty_fp, int tty_fd)
{
	extern keybinding keybindings[];

	int mode = PAGER;
	size_t i = 0;
	char k;

	/* main execution loop: get input char by char from the keyboard */
	while (mode != EXIT_FAILURE && mode != EXIT_SUCCESS) {

		k = fgetc(tty_fp);

		if (mode == PAGER)
			input_multiplier(k, tty_fp);

		/* execute action associated with key */
		for (i = 0; i < LENGTH(keybindings); i++) {
			if (keybindings[i].keys[0] == k)
				keybindings[i].action();
		}
	}

	return mode;
}
