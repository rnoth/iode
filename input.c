#include <stdio.h>
#include <stdlib.h>

#include "main.h"


/*
 * Perform action associated with key
 */
int
input_key(FILE *tty_fp)
{
	switch (fgetc(tty_fp)) {
	case CONTROL('C'):
		return EXIT_FAILURE;
	default:
		return CONTINUE;
	}
}


/*
 * Listen for the user input and call the appropriate functions.
 */
int
input_get(FILE *tty_fp)
{
	int exit_code;

	/* main execution loop: get input char by char from the keyboard */
	while ((exit_code = input_key(tty_fp)) == CONTINUE);

	return exit_code;
}
