#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "main.h"


/*
 * Set `c` content to a representation of the first char of `text`, then it
 * shifts `text` pointer to next char.
 *
 * Return: onscreen width of the char while printed.
 */
int
draw_char(char **character, char **text, int col)
{
	int i = 0, width = 0;  /* onscreen width of the char to print */
	char *c = *character, *t = *text;

	if (ISASCII(t[0])) {
		if (isprint(t[0])) {
			c[0] = t[0]; c[1] = '\0';
			width = 1;

		} else if (t[0] == '\t') {
			c[0] = '\t'; c[1] = '\0';
			width = 8 - (col % 8);

		} else if (ISCONTROL(t[0])) {
			sprintf(c, "\033[1;30m^%c\033[0m", t[0] + '@');
			width = 2;

		} else {
			sprintf(c, "\033[1;30m??\033[0m");
			width = 2;
		}

		i++;
	} else {
		for (i = 0; t[i] && ISUTF8(t[i]); i++)
			c[i] = t[i];
		c[i + 1] = '\0';
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
draw_line(Line *line, int cols)
{
	int col;
	/*                4 max size for UTF-8
	 *               11 "\033[1;3%sm" and "\033[0m"
	 *                1 null byte */
	char *c = malloc(16 * sizeof(char)), *text = line->text;

	fputs("\033[K", stderr);

	/* draw chars until the screen is filled or end of string */
	for (col = 0; text[0] && cols - col > 0;) {
		col += draw_char(&c, &text, col);
		/* now `c` is set and `text` points to the next char */
		fputs(c, stderr);

		/* not enough space to fit next char onscreen */
		if (cols - col < 2 && text[0] && text[1]) {
			fputs("\033[1;31m>\033[0m", stderr);
			break;
		}
	}

	fputc('\n', stderr);
	
	free(c);
}


void
draw_status_line(Buffer *buffer, int cols)
{
	fprintf(stderr, "\033[30;47m\033[K%7d:%d\033[0m", buffer->l, buffer->c);
}


/*
 * Draw the full interface
 */
void
draw_screen(Buffer *buffer, int rows, int cols)
{
	Line *line = NULL;
		
	fputs("\033[s", stderr);
	
	for (line = buffer->top; line && rows > 1; line = line->next, rows--)
		draw_line(line, cols);

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
	if (!buffer->top->prev)
		return;

	buffer->top = buffer->top->prev;

	/* save cursor position, go to at the bottom, scroll one line up */
	fputs("\033[s\033[0;0H\033M", stderr);

	draw_line(buffer->top, cols);

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

	if (!buffer->top->next || !line)
		return;

	buffer->top = buffer->top->next;

	/* save cursor position, go to the bottom */
	fprintf(stderr, "\033[s\033[%d;0H", rows);


	draw_line(line, cols);
	draw_status_line(buffer, cols);

	fputs("\033[u", stderr);
}
