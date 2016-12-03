#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Perform action associated with key
 *
 * TODO: use goto to switch between different pending modes and normal mode?
 */
int
input_key(FILE *tty_fp, int tty_fd, Buffer *buffer)
{
	int    i, m = 0;
	struct winsize w;
	char   key = fgetc(tty_fp);

	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	while (key <= '9' && key >= '0' && m < 1000) {
		m = m * 10 + (key - '0');
		sprintf(buffer->operators, "%d", m ? m : 0);
		update_status_line(buffer, w.ws_row, w.ws_col);
		key = fgetc(tty_fp);
	}

	switch (key) {

	case 'q':
	case CONTROL('C'):
		return EXIT_FAILURE;

	case CONTROL('L'):
		draw_screen(buffer, w.ws_row, w.ws_col);
		break;

	case 'g':
		buffer->top   = buffer->first;
		buffer->top_l = 1;

		while (buffer->top_l < m && buffer->top->next) {
			buffer->top = buffer->top->next;
			buffer->top_l++;
		}

		draw_screen(buffer, w.ws_row, w.ws_col);
		break;

	case 'G':
		buffer->top   = buffer->last;
		buffer->top_l = buffer->total;

		while (buffer->top->prev
			&& buffer->total - buffer->top_l + 1
				< (m ? m : w.ws_row) - 1) {

			buffer->top = buffer->top->prev;
			buffer->top_l--;
		}

		draw_screen(buffer, w.ws_row, w.ws_col);
		break;

	case 'u':
		for (i = 0; i < (w.ws_row - 1) / 2 * (m ? m : 1); i++)
			scroll_up(buffer, w.ws_row, w.ws_col);
		break;

	case 'd':
		for (i = 0; i < (w.ws_row - 1) / 2 * (m ? m : 1); i++)
			scroll_down(buffer, w.ws_row, w.ws_col);
		break;

	case 'f':
		for (i = 0; i < (w.ws_row - 2) * (m ? m : 1); i++)
			scroll_down(buffer, w.ws_row, w.ws_col);
		break;

	case 'b':
		for (i = 0; i < (w.ws_row - 2) * (m ? m : 1); i++)
			scroll_up(buffer, w.ws_row, w.ws_col);
		break;

	case 'j':
		for (i = 0; i < (m ? m : 1); i++)
			scroll_down(buffer, w.ws_row, w.ws_col);
		break;

	case 'k':
		for (i = 0; i < (m ? m : 1); i++)
			scroll_up(buffer, w.ws_row, w.ws_col);
		break;
	}

	buffer->operators[0] = '\0';
	update_status_line(buffer, w.ws_row, w.ws_col);

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
