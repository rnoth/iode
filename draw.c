#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "main.h"


/*
 * Set `c` content to a representation of the first char of `text`, then it
 * shifts `text` pointer to next char.
 *
 * Return: onscreen width of the char while printed.
 *
 * In comments, "char" refers to the eight bits that compose a char, and
 * "character" is the single or multiple character considered as one character.
 */
int
draw_char(char **character, char **text, int col, int vis)
{
	int i     = 0;  /* how many chars did we read to render one character */
	int width = 0;  /* how much screen width is this character taking */
	char *c = *character, *t = *text;

	if (ISASCII(t[0])) {

		if (isprint(t[0])) {
			c[0] = t[0]; c[1] = '\0';
			width = 1;
			i = 1;

		/* mandoc adds '^H' between some characters */
		} else if ((!vis && t[0] == CONTROL('H'))) {
			c[0] = t[0]; c[1] = '\0';
			width = -1;
			i = 1;

		/* tab */
		} else if (t[0] == '\t') {
			c[0] = '\t'; c[1] = '\0';
			width = 8 - (col % 8);
			i = 1;

		/* escape codes */
		} else if (!vis && (t[0] == '\033')) {
			for (i = 0; t[i] && !isalpha(t[i]); i++)
				c[i] = t[i];
			c[i] = '\0';
			/* next char is the alpha that ends escape code */
			width = -1;

		/* control characters */
		} else if (ISCONTROL(t[0])) {
			sprintf(c, "\033[1;30m^%c\033[m", t[0] + '@');
			width = 2;
			i = 1;

		/* unknown */
		} else {
			sprintf(c, "\033[1;30m?\033[m");
			width = 1;
			i = 1;
		}

	} else {
		for (i = 0; t[i] && ISUTF8(t[i]); i++)
			c[i] = t[i];
		c[i] = '\0';
		width = 1;
	}

	*character = c;
	*text = &t[i];
	return width;
}


/*
 * Draw a full line up to the width of the screen.
 */
void
draw_line(Line *line, int cols, int number)
{
	int col = 0;
	/*                4 max size for UTF-8
	 *               11 "\033[1;3%sm" and "\033[m"
	 *                1 null byte */
	char *c = malloc(16 * sizeof(char)), *text = line->text;

	fprintf(stderr, "\033[K\033[1;30m%7d\033[m ", number);
	col += 8;

	/* draw chars until the screen is filled or end of string */
	for (; text[0] && cols - col > 0;) {
		col += draw_char(&c, &text, col, 0);
		/* now `c` is set and `text` points to the next char */

		fputs(c, stderr);

		/* not enough space to fit next char onscreen */
		if (cols - col < 2 && text[0] && text[1]) {
			fputs("\033[1;31m>\033[m", stderr);
			break;
		}
	}

	fputc('\n', stderr);
	
	free(c);
}


void
draw_status_line(Buffer *buffer, int cols)
{
	fprintf(stderr, "\033[1m\033[K\033[%dC%s\r%7d\033[m",
		cols - 20 - (int) strlen(buffer->operators),
		buffer->operators, buffer->total);
}


/*
 * Draw the status line without changing the cursor position
 */
void
update_status_line(Buffer *buffer, int rows, int cols)
{
	fprintf(stderr, "\033[s\033[%d;0H", rows);
	draw_status_line(buffer, cols);
	fputs("\033[u", stderr);
}


/*
 * Draw the full interface
 */
void
draw_screen(Buffer *buffer, int rows, int cols)
{
	Line *line   = buffer->top;
	int   number = buffer->top_l;
		
	fputs("\033[s\033[H", stderr);
	
	for (; line && rows > 1; line = line->next, rows--, number++)
		draw_line(line, cols, number);

	for (; rows > 1; rows--)
		fputs("      \033[1;30m.\033[m\033[K\n", stderr);

	draw_status_line(buffer, cols);

	fputs("\033[u", stderr);
}


/*
 * Scroll up one line using escape sequences, print the new line, then print
 * the status line.
 */
void
scroll_up(Buffer *buffer, int rows, int cols)
{
	if (!buffer->top || !buffer->top->prev)
		return;

	buffer->top = buffer->top->prev;

	/* save cursor position, go to at the bottom, scroll one line up */
	fputs("\033[s\033[0;0H\033M", stderr);

	draw_line(buffer->top, cols, --buffer->top_l);

	/* go to the bottom */
	fprintf(stderr, "\033[%d;0H", rows);

	draw_status_line(buffer, cols);

	fputs("\033[u", stderr);
}


/*
 * Scroll down one line by printing a new line, then print the status line.
 */
void
scroll_down(Buffer *buffer, int rows, int cols)
{
	int   i;
	Line *line = buffer->top;

	for (i = 0; i < rows - 1 && line; i++, line = line->next);

	if (!buffer->top || !buffer->top->next || !line)
		return;

	buffer->top = buffer->top->next;
	buffer->top_l++;

	/* save cursor position, go to the bottom */
	fprintf(stderr, "\033[s\033[%d;0H", rows);

	draw_line(line, cols, buffer->top_l + rows - 2);
	draw_status_line(buffer, cols);

	fputs("\033[u", stderr);
}
