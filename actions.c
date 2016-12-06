#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"


/*
 * a->b:  buffer
 * a->m:  multiplier
 * a->r:  a->r
 * a->c:  a->c
 */


int
a_quit()
{
	return EXIT_SUCCESS;
}


int
a_redraw(Arg *a)
{
	draw_screen(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_jump_begin(Arg *a)
{
	a->b->top   = a->b->first;
	a->b->top_l = 1;

	while (a->b->top && a->b->top_l < a->m && a->b->top->next) {
		a->b->top = a->b->top->next;
		a->b->top_l++;
	}

	draw_screen(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_jump_end(Arg *a)
{
	a->b->top   = a->b->last;
	a->b->top_l = a->b->total;

	while (a->b->top
		&& a->b->top->prev
		&& a->b->total - a->b->top_l + 1
			< (a->m ? a->m : a->r) - 1) {

		a->b->top = a->b->top->prev;
		a->b->top_l--;
	}

	draw_screen(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_half_page_up(Arg *a)
{
	int i;

	for (i = 0; i < (a->r - 1) / 2 * (a->m ? a->m : 1); i++)
		scroll_up(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_half_page_down(Arg *a)
{
	int i;

	for (i = 0; i < (a->r - 1) / 2 * (a->m ? a->m : 1); i++)
		scroll_down(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_page_up(Arg *a)
{
	int i;

	for (i = 0; i < (a->r - 2) * (a->m ? a->m : 1); i++)
		scroll_up(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_page_down(Arg *a)
{
	int i;

	for (i = 0; i < (a->r - 2) * (a->m ? a->m : 1); i++)
		scroll_down(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_scroll_up(Arg *a)
{
	int i;

	for (i = 0; i < (a->m ? a->m : 1); i++)
		scroll_up(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_scroll_down(Arg *a)
{
	int i;

	for (i = 0; i < (a->m ? a->m : 1); i++)
		scroll_down(a->b, a->r, a->c);

	return CONTINUE;
}


int
a_editor(Arg *a)
{
	char *editor = getenv("EDITOR"), command[MAX_LINE_SIZE];
	char *filename = a->b->filename;
	FILE *file     = a->b->file;

	if (!filename)
		return CONTINUE;

	/* open filename with "$EDITOR" at line a->b->top_l */
	fputs("\033[?6c", stderr);
	sprintf(command, "%s +%d %s", editor, a->b->top_l, filename);
	if (system(command))
		return EXIT_FAILURE;
	fputs("\033[?12c", stderr);

	/* read the file again to keep up with the changes */
	buffer_free(a->b);
	fclose(file);
	a->b = buffer_read(filename);

	draw_screen(a->b, a->r, a->c);

	return CONTINUE;
}
