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
	int u = 0;

	if (ISASCII(t[0])) {

		if (isprint(t[0])) {
			width = 1;
			i = 1;

		/* tab */
		} else if (t[0] == '\t') {
			c[0] = '\t'; c[1] = '\0';
			width = 8 - (col % 8);
			i = 1;

		/* control characters */
		} else if (iscntrl(t[0])) {
			sprintf(c, "\033[36;1m^%c\033[m", t[0] + '@');
			width = 2;
			i = 1;
		}

	} else {
		/* unicode parsing */
		for (u = 0; (t[i] & (1 << (7 - u))) && u < 7; u++)
			;

		if (u < 4) {
			for (; u > 0; u--, i++)
				c[i] = t[i];
			c[i] = '\0';
			width = 1;
		} else {
			sprintf(c, "\033[36;1m%02x\033[m",
				(unsigned char) t[i]);
			width = 2;
			i = 1;
		}
	}

	*character = c;
	*text = &t[i];
	return width;
}


/*
 * Draw a full line up to the width of the screen.
 */
void
draw_line(struct line *line, int number)
{
	extern struct line *l_current;
	extern int cols;

	int col = 0;
	/*                4 max size for UTF-8
	 *               11 "\033[1;3%sm" and "\033[m"
	 *                1 null byte */
	char *c = NULL, *text = NULL;

	if (!line) {
		fputs("\r      \033[1;30m.\033[m\033[K\n", stderr);
		return;
	}

	fprintf(stderr, line == l_current ?  "\r\033[K\033[1m%7d\033[m " :
		"\r\033[K\033[1;30m%7d\033[m ", number);
	col += 8;

	/* draw chars until the screen is filled or end of string */
	c    = malloc(16 * sizeof(char));
	text = line->text;
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


/*
 * Update one line without changing the cursor.
 */
void
update_line(struct line *line, int number)
{
	extern int n_top, rows;

	/* line is outside of the screen */
	if (number - (n_top - 1) > rows)
		return;

	fprintf(stderr, "\033[s\033[%d;0H", number - (n_top - 1));
	draw_line(line, number);
	fputs("\033[u", stderr);
}


/*
 * Draw the status line using current editor state.
 */
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


/*
 * Update onscreen cursor position to current cursor position.
 */
void
update_cursor()
{
	extern int n_current, n_top;

	fprintf(stderr, "\033[%d;%dH", n_current - (n_top - 1), 8 + 1);
}


/*
 * Draw the full interface
 */
void
draw_screen(void)
{
	extern struct line *l_top;
	extern int   n_top, rows;

	struct line *l = l_top;
	int n, r;

	update_terminal_size();
	n = n_top; r = rows;

	fputs("\033[s\033[H", stderr);

	for (; r > 1; l = l ? l->next : NULL, r--, n++)
		draw_line(l, n);

	draw_status_line();

	fputs("\033[u", stderr);

	update_cursor();
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
 * Move cursor until it fits inside the screen.
 */
void
cursor_follow_screen()
{
	extern struct line *l_current;
	extern int n_top, n_current;

	/* move cursor if it is above the screen */
	while (n_current < n_top) {
		l_current = l_current->next;
		n_current++;
	}

	/* move cursor if it is under the screen */
	while (n_current > n_top + rows - 2) {
		l_current = l_current->prev;
		n_current--;
	}

	update_line(l_current, n_current);
	update_cursor();
}


/*
 * Scroll up one line using escape sequences, print the new line, then print
 * the status line.
 */
void
scroll_up(void)
{
	extern struct line *l_top;
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
	extern struct line *l_top;
	extern int rows;

	int i;
	struct line *line = l_top;

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


/*
 * Move n_top and l_top to make the cursor fit inside the visible region.
 */
void
screen_focus_cursor()
{
	extern struct line *l_current;
	extern int n_top, n_current, rows;

	/* reach cursor if it is above the screen */
	while (l_current && n_current < n_top)
		scroll_up();

	/* reach cursor if it is under the screen */
	while (l_current && n_current > n_top + rows - 2)
		scroll_down();

	update_cursor();
	update_line(l_current, n_current);
}


/*
 * Move cursor up one line, and update the screen accordingly if needed.
 */
void
cursor_up(void)
{
	extern struct line *l_current;
	extern int n_current;

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
	extern int n_current;

	if (!l_current || !l_current->next)
		return;

	l_current = l_current->next;
	n_current++;

	screen_focus_cursor();
	update_line(l_current->prev, n_current - 1);
	update_line(l_current, n_current);
	update_cursor();
}
