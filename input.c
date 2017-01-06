/*
 * Main execution loop.
 *
 * Getting one char at the time from the user input and calling appropriate
 * function according to the input.
 *
 * The functions called are in keybindings[] in keybindings.h, and the
 * function possible to call are in actions.c.
 *
 * The input() relies on other functions to fully cover a vi-like modal
 * input and edit style.
 */


#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "keybindings.h"


/*
 * Apply a modifier to the argument, to pass to commands.
 */
char
get_multiplier(char k, FILE *tty_fp)
{
	extern int multiplier;
	extern char operators[];

	multiplier = 0;
	while (k >= '0' && k <= '9' && multiplier < 1000) {
		multiplier = multiplier * 10 + (k - '0');
		sprintf(operators, "%d", multiplier);
		update_status_line();
		k = fgetc(tty_fp);
	}

	operators[0] = '\0';
	update_status_line();

	return k;
}


/*
 * Listen for the user input and call the appropriate functions.
 *
 * Main execution loop.
 */
int
input()
{
	extern struct keybinding keybindings[];
	extern int multiplier, mode;

	size_t i = 0;
	char k;
	FILE *tty_fp = NULL;
	
	if (!(tty_fp = fopen("/dev/tty", "r")))
		die("fopen");

	/* main execution loop: get input char by char from the keyboard */
	mode = PAGER;
	while (mode != EXIT_FAILURE && mode != EXIT_SUCCESS) {

		k = fgetc(tty_fp);

		if (mode == PAGER)
			k = get_multiplier(k, tty_fp);

		/* execute action associated with key */
		for (i = 0; i < LENGTH(keybindings); i++) {
			if (keybindings[i].keys[0] == k)
				keybindings[i].action();
		}
	}

	if (fclose(tty_fp) == EOF)
		die("fclose");

	return mode;
}
