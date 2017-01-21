/*
 * Functions called on demand of the user.
 *
 * They are called from input() in "input.c" while the user input the
 * apropriate key sequence.
 *
 * The mapping between key sequences and actions is done with keybindings[]
 * in "keybindings.h".
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"


void
a_quit()
{
	extern int mode;

	mode = EXIT_SUCCESS;
}


void
a_redraw(void)
{
	draw_screen();
}


void
a_jump_begin(void)
{
	extern struct line *l_current, *l_first;
	extern size_t multiplier, n_current;

	struct line *line = l_current;
	size_t number = n_current;

	if (!l_current)
		return;

	l_current = l_first;
	n_current = 1;

	update_line(line, number);

	while (n_current < multiplier && l_current->next) {
		l_current = l_current->next;
		n_current++;
	}

	screen_focus_cursor();
}


void
a_jump_end(void)
{
	extern struct line *l_current, *l_last;
	extern size_t multiplier, n_current, n_total;

	struct line *line = l_current;
	size_t number = n_current;

	if (!l_current)
		return;

	l_current = l_last;
	n_current = n_total;

	update_line(line, number);

	while (n_current > n_total - multiplier && l_current->prev) {
		l_current = l_current->prev;
		n_current--;
	}

	screen_focus_cursor();
}


void
a_half_page_up(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (rows - 1) / 2 * (multiplier ? multiplier : 1); i++)
		scroll_up();

	cursor_follow_screen();
}


void
a_half_page_down(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (rows - 1) / 2 * (multiplier ? multiplier : 1); i++)
		scroll_down();

	cursor_follow_screen();
}


void
a_page_up(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (rows - 2) * (multiplier ? multiplier : 1); i++)
		scroll_up();

	cursor_follow_screen();
}


void
a_page_down(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (rows - 2) * (multiplier ? multiplier : 1); i++)
		scroll_down();

	cursor_follow_screen();
}


void
a_scroll_up(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		scroll_up();

	cursor_follow_screen();
}


void
a_scroll_down(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		scroll_down();

	cursor_follow_screen();
}


void
a_cursor_down(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		cursor_down();

	screen_focus_cursor();
}


void
a_cursor_up(void)
{
	extern size_t multiplier;

	size_t i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		cursor_up();

	screen_focus_cursor();
}


void
a_editor(void)
{
	extern char   *filename;
	extern FILE   *file;
	extern size_t  n_top;

	size_t top = n_top;
	char *editor = getenv("EDITOR"), command[MAX_LENGTH];

	if (!filename)
		return;

	/* open filename with "$EDITOR" at line n_top */
	fputs("\033[?6c", stderr);
	sprintf(command, "%s +%ld %s", editor, n_top, filename);
	if (system(command))
		return;
	fputs("\033[?12c", stderr);

	/* read the file again to keep up with the changes */
	free_buffer(l_first);
	fclose(file);
	read_buffer(filename);

	for (; top > 1; top--)
		scroll_down();
	cursor_follow_screen();

	draw_screen();
}


void
a_add_flag()
{
	extern FILE *tty_fp;

	set_flag(fgetc(tty_fp), 1);
}


void
a_remove_flag()
{
	extern FILE *tty_fp;

	set_flag(fgetc(tty_fp), 0);
}
