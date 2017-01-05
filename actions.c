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
	extern int multiplier;
	extern struct line *l_top, *l_first;

	l_top = l_first;
	n_top = 1;

	draw_screen();

	while (l_top && n_top < multiplier && l_top->next)
		scroll_down();
}


void
a_jump_end(void)
{
	extern struct line *l_top, *l_last;
	extern int n_top, n_total;
	extern int multiplier;

	l_top = l_last;
	n_top = n_total;

	draw_screen();

	while (l_top && l_top->prev && n_top > n_total - multiplier + 1)
		scroll_up();
}


void
a_half_page_up(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (rows - 1) / 2 * (multiplier ? multiplier : 1); i++)
		scroll_up();
}


void
a_half_page_down(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (rows - 1) / 2 * (multiplier ? multiplier : 1); i++)
		scroll_down();
}


void
a_page_up(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (rows - 2) * (multiplier ? multiplier : 1); i++)
		scroll_up();
}


void
a_page_down(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (rows - 2) * (multiplier ? multiplier : 1); i++)
		scroll_down();
}


void
a_scroll_up(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		scroll_up();
}


void
a_scroll_down(void)
{
	extern int multiplier;

	int i;

	for (i = 0; i < (multiplier ? multiplier : 1); i++)
		scroll_down();
}


void
a_editor(void)
{
	extern char *filename;
	extern FILE *file;
	extern struct line *l_top;
	extern int   n_top;

	char *editor = getenv("EDITOR"), command[MAX_LINE_SIZE];

	if (!filename)
		return;

	/* open filename with "$EDITOR" at line n_top */
	fputs("\033[?6c", stderr);
	sprintf(command, "%s +%d %s", editor, n_top, filename);
	if (system(command))
		return;
	fputs("\033[?12c", stderr);

	/* read the file again to keep up with the changes */
	free_buffer(l_first);
	fclose(file);
	read_buffer(filename);

	draw_screen();
}
