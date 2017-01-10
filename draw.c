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
 * Fill `*str` with a visual representation for `rune[4]` ready to be printed.
 */
int
rune_to_printable(char *str, long rune, int col)
{
	char s[4] = {'\0', '\0', '\0', '\0'};

	rune_to_str(s, rune);

	if (iscntrl(s[0]) && s[0] != '\t') {
		sprintf(str, "\033[36;1m^%s\033[m", "!");  /* TODO */
		return 2;

	} else if (ISASCII(s[0]) || s[1]) {
		strncpy(str, s, 4);
		str[5] = '\0';

		if (s[0] == '\t')
			return 8 - (col % 8);
		return 1;
	}

	/* unknown */
	sprintf(str, "\033[36m%02x\033[m", (unsigned char) s[0]);
	return 2;
}


/*
 * Draw a full line up to the width of the screen.
 */
void
draw_line(struct line *line, int number)
{
	extern struct line *l_current;
	extern int cols;

	/* 18 is length of "\033[3?m????\033[m" */
	char s[5] = "", str[18 * MAX_LINE_SIZE + 1] = "";
	int i, col = 8;

	if (!line) {
		fputs("\r      \033[1;30m.\033[m\033[K\n", stderr);
		return;
	}

	/* add chars to print until the screen is filled or end of string */
	for (i = 0; line->text[i] && cols - col > 0; i++) {
		col += rune_to_printable(s, line->text[i], col);

		/* not enough space to fit next char onscreen */
		if (
			cols < col ||
			(cols == col && line->text[i] && line->text[i + 1])
		) {
			strcat(str, "\033[1;31m>\033[m");
			break;
		}

		strcat(str, s);
	}

	fprintf(
		stderr,
		line == l_current ?
		"\r\033[K\033[1m"    "%7d\033[m %s\n" :
		"\r\033[K\033[1;30m" "%7d\033[m %s\n",
		number, str
	);
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
