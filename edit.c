/*
 * Text editing functions.
 *
 * Some text operations are based off a visual selection that should be set
 * before calling the corresponding functions.
 */


#include <stdlib.h>
#include <stdio.h>

#include "main.h"


/*
 * Move cursor up one line, and update the screen accordingly if needed.
 */
void
cursor_up(void)
{
	extern struct line *l_current;
	extern size_t n_current;

	if (!l_current || !l_current->prev)
		return;

	l_current = l_current->prev;
	n_current--;

	screen_focus_cursor();
	update_line(l_current, n_current);
	update_line(l_current->next, n_current + 1);
	update_cursor();
}


void
cursor_down(void)
{
	extern struct line *l_current;
	extern size_t n_current;

	if (!l_current || !l_current->next)
		return;

	l_current = l_current->next;
	n_current++;

	screen_focus_cursor();
	update_line(l_current->prev, n_current - 1);
	update_line(l_current, n_current);
	update_cursor();
}
