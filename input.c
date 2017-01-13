/*
 * Main execution loop.
 *
 * Getting one char at the time from the user input and calling appropriate
 * function according to the input.
 *
 * The functions called are in keys[] in keys.h, and the
 * function possible to call are in actions.c.
 *
 * The input() relies on other functions to fully cover a vi-like modal
 * input and edit style.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "keys.h"


/*
 * Apply a modifier to the argument, to pass to commands.
 */
char
get_key(FILE *tty_fp)
{
	extern int multiplier;
	extern char input[];

	char key = fgetc(tty_fp);

	multiplier = 0;
	while (key >= '0' && key <= '9' && multiplier < 1000) {
		multiplier = multiplier * 10 + (key - '0');
		sprintf(input, "%d", multiplier);
		update_status_line();
		key = fgetc(tty_fp);
	}

	return key;
}


/*
 * Listen for the user input and call the appropriate functions.
 *
 * Main execution loop.
 */
int
get_input()
{
	extern *(void) keys[];
	extern int multiplier, mode;

	FILE *tty_fp = NULL;

	if (!(tty_fp = fopen("/dev/tty", "r")))
		die("fopen");

	/* main execution loop: get input char by char from the keyboard */
	mode = PAGER;
	while (mode != EXIT_FAILURE && mode != EXIT_SUCCESS) {
		char key = get_key(tty_fp);

		if (keys[mode][(int) key])
			keys[mode][(int) key]();
	}

	if (fclose(tty_fp) == EOF)
		die("fclose");

	return mode;
}
