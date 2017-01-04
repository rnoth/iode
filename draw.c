/*
 * Functions for darwing the interface using vt100 escape sequence.
 *
 * Through this code, "char" refers to the eight bits that compose a char, and
 * "character" is the single or multiple character considered as one character.
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Set `c` content to a representation of the first character of `text`, then it
 * shifts `text` pointer to next char.
 *
 * Return: onscreen width of the char while printed.
 */
int
draw_char(char **character, char **text, int col)
{
	int i     = 0;  /* how many chars did we read to render one character */
	int width = 0;  /* how much screen width is this character taking */
	char *c = *character, *t = *text;

	if (ISASCII(t[0])) {

		if (isprint(t[0])) {
			c[0] = t[0]; c[1] = '\0';
			width = 1;
			i = 1;

		/* mandoc adds '^H' between each heading characters */
		} else if ((t[0] == CONTROL('H'))) {
			c[0] = t[0]; c[1] = '\0';
			width = -1;
			i = 1;

		/* tab */
		} else if (t[0] == '\t') {
			c[0] = '\t'; c[1] = '\0';
			width = 8 - (col % 8);
			i = 1;

		/* escape codes */
		} else if ((t[0] == '\033')) {
			for (i = 0; t[i] && !isalpha(t[i]); i++)
				c[i] = t[i];
			c[i] = '\0';
			/* next char is the alpha that ends escape code */
			width = -1;

		/* control characters */
		} else if (iscntrl(t[0])) {
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
draw_line(Line *line, int number)
{
	extern int cols;

	int col = 0;
	/*                4 max size for UTF-8
	 *               11 "\033[1;3%sm" and "\033[m"
	 *                1 null byte */
	char *c = malloc(16 * sizeof(char)), *text = line->text;

	if (!line) {
		fputs("      \033[1;30m.\033[m\033[K\n", stderr);
		return;
	}

	fprintf(stderr, "\033[K\033[1;30m%7d\033[m ", number);
	col += 8;

	/* draw chars until the screen is filled or end of string */
	for (; text[0] && cols - col > 0;) {
		col += draw_char(&c, &text, col);

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
draw_status_line(void)
{
	extern char *filename, operators[];
	extern int n_total, cols;

	fprintf(stderr, "\033[1m\033[K\033[%dC%s\r%7d - %s\033[m",
		cols - 20 - (int) strlen(operators),
		operators, n_total, filename);
}


/*
 * Draw the full interface
 */
void
draw_screen(void)
{
	extern Line *l_top;
	extern int   n_top, rows;

	Line *l = l_top;
	int   n = n_top, r = rows;

	update_terminal_size();

	fputs("\033[s\033[H", stderr);

	for (; r > 1; l = l ? l->next : NULL, r--, n++)
		draw_line(l, n);

	draw_status_line();

	fputs("\033[u", stderr);
}


/*
 * Draw the status line without changing the cursor position
 */
void
update_status_line(void)
{
	extern int rows;

	fprintf(stderr, "\033[s\033[%d;0H", rows);
	draw_status_line();
	fputs("\033[u", stderr);
}


void
update_terminal_size(void)
{
	extern int rows, cols;

	struct winsize w;

	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	rows = w.ws_row;
	cols = w.ws_col;
}


/*
 * Scroll up one line using escape sequences, print the new line, then print
 * the status line.
 */
void
scroll_up(void)
{
	extern Line *l_top;
	extern int rows, cols;

	if (!l_top || !l_top->prev)
		return;
	l_top = l_top->prev;

	fputs("\033[s\033[0;0H\033M", stderr);
	draw_line(l_top, --n_top);

	fprintf(stderr, "\033[%d;0H", rows);
	draw_status_line();
	fputs("\033[u", stderr);
}


/*
 * Scroll down one line by printing a new line, then print the status line.
 */
void
scroll_down(void)
{
	extern Line *l_top;
	extern int rows;

	int i;
	Line *line = l_top;

	for (i = 0; i < rows - 1 && line; i++, line = line->next);

	if (!l_top || !l_top->next)
		return;

	l_top = l_top->next;
	n_top++;

	fprintf(stderr, "\033[s\033[%d;0H", rows);
	draw_line(line, n_top + rows - 2);

	draw_status_line();

	fputs("\033[u", stderr);
}
