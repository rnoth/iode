/*
 * Functions for darwing the interface using vt100 escape sequences.
 */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/ioctl.h>

#include "main.h"


/*
 * Check if `rune[0]` is an escape sequence, and if so, fill str with it and
 * return the length of the escape sequence.
 */
int
escape(char *str, long rune[], size_t *col) {

	if (!flags['R']) {
		return 0;

	/* ansi secape sequence "\033[[^A-Za-z][A-Za-z]" */
	} else if (rune[0] == '\033') {
		int i = 0;
		for (i = 0; rune[0] < 0x80 && !isalpha(rune[i]); i++)
			str[i] = (char) rune[i];

		str[i + 1] = rune[i + 1];
		str[i + 2] = '\0';

		(*col)--;
		return i;

	/* ^H bold */
	} else if (rune[1] == '\b' && rune[0] == rune[2] && rune[0]) {
		sprintf(str, "\033[1m%c\033[m", (char) rune[0]);

		(*col)++;
		return 3;

	/* ^H underline */
	} else if (rune[1] == '\b' && rune[0] == '_') {
		sprintf(str, "\033[4m%c\033[m", (char) rune[2]);

		(*col)++;
		return 3;

	/* non-breaking space */
	} else if (rune[0] == 0xa0) {
		str[0] = ' ';
		(*col)++;
		return 1;
	}

	return 0;
}


/*
 * Return a printable form of line that fits into the screen.
 */
void
line_printable(char str[], struct line *line, size_t col)
{
	extern size_t cols;
	extern int flags[];

	size_t i;

	/* print `line` until the screen is filled */
	for (i = 0; i < line->length && cols - col > 0; i++) {
		char s[18] = "";
		int  esc = 0, print = 1;

		if ((esc = escape(s, &line->text[i], &col))) {
			i += esc - 1;

		} else if ((print = utf8_printable(s, line->text[i]))) {
			col +=(line->text[i] == '\t') ?  8 - col % 8 : 1;
		} else {
			col += strlen(s);
		}

		/* not enough space to fit next char onscreen */
		if (col > cols || (col == cols && line->length > i)) {
			strcat(str, "\033[1;31m>\033[m");
			break;
		}

		if (print) {
			strcat(str, s);
		} else {
			sprintf(str + strlen(str), "\033[36m%s\033[m", s);
		}
	}
}


/*
 * Draw a full line up to the width of the screen.
 */
void
draw_line(struct line *line, size_t number)
{
	extern struct line *l_current;
	extern size_t cols;
	extern int flags[];

	/* 18 is length of "\033[1;3?m????????\033[m" */
	char str[18 * MAX_LENGTH + 1] = "";
	size_t col = flags['N'] ? 8 : 0;

	if (!line) {
		fputs(
			flags['N'] ?
				"\r      \033[1;30m.\033[m" "\033[K\n" :
				"\r"                        "\033[K\n", stderr
		);
		return;
	}

	line_printable(str, line, col);

	if (flags['N']) {
		fprintf(
			stderr,
			line == l_current ?
				"\r\033[K\033[1m"    "%7ld\033[m %s\n" :
				"\r\033[K\033[1;30m" "%7ld\033[m %s\n",
			number, str
		);
	} else {
		fprintf(stderr, "\r\033[K%s\n", str);
	}
}


/*
 * Update one line without changing the cursor.
 */
void
update_line(struct line *line, size_t number)
{
	extern size_t n_top, rows;

	/* line is outside of the screen */
	if (number - (n_top - 1) > rows)
		return;

	fprintf(stderr, "\033[%ld;0H", number - (n_top - 1));
	draw_line(line, number);
	update_cursor();
}


/*
 * Draw the status line using current editor state.
 */
void
draw_status_line(void)
{
	extern char *filename, input[];
	extern size_t n_total, cols;
	extern struct line *l_current;

	fprintf(stderr, "\033[1m\033[K\033[%ldC%s\r%7ld:%-9ld %s\033[m",
		cols - 20 - strlen(input),
		input,
		n_total,
		l_current ? l_current->length : 0,
		filename
	);
}


/*
 * Draw the status line without changing the cursor position
 */
void
update_status_line(void)
{
	extern size_t rows;

	fprintf(stderr, "\033[%ldH", rows);
	draw_status_line();
}


/*
 * Update onscreen cursor position to current cursor position.
 */
void
update_cursor()
{
	extern size_t n_current, n_top;

	fprintf(stderr, "\033[%ld;%dH", n_current - (n_top - 1), 8 + 1);
}


/*
 * Draw the full interface
 */
void
draw_screen(void)
{
	extern struct line *l_top;
	extern size_t   n_top, rows;

	struct line *l = l_top;
	size_t n, r;

	update_terminal_size();
	n = n_top; r = rows;

	fputs("\033[H", stderr);

	for (; r > 1; l = l ? l->next : NULL, r--, n++)
		draw_line(l, n);

	draw_status_line();

	update_cursor();
}


void
update_terminal_size(void)
{
	extern size_t rows, cols;

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
	extern struct line *l_top;
	extern size_t rows, cols;

	if (!l_top || !l_top->prev)
		return;
	l_top = l_top->prev;

	fputs("\033[H\033M", stderr);
	draw_line(l_top, --n_top);
}


/*
 * Scroll down one line by printing a new line, then print the status line.
 */
void
scroll_down(void)
{
	extern struct line *l_top;
	extern size_t rows;

	size_t i;
	struct line *line = l_top;

	for (i = 0; i < rows - 1 && line; i++, line = line->next);

	if (!l_top || !l_top->next)
		return;

	l_top = l_top->next;
	n_top++;

	fprintf(stderr, "\033[%ldH", rows);
	draw_line(line, n_top + rows - 2);
}


/*
 * Move n_top and l_top to make the cursor fit inside the visible region.
 */
void
screen_focus_cursor()
{
	extern struct line *l_current;
	extern size_t n_top, n_current, rows;

	/* reach cursor if it is above the screen */
	while (l_current && n_current < n_top)
		scroll_up();

	/* reach cursor if it is under the screen */
	while (l_current && n_current > n_top + rows - 2)
		scroll_down();

	update_status_line();
	update_cursor();
	update_line(l_current, n_current);
}


/*
 * Move cursor until it fits inside the screen.
 */
void
cursor_follow_screen()
{
	extern struct line *l_current;
	extern size_t n_top, n_current;

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

	update_status_line();
	update_line(l_current, n_current);
	update_cursor();
}
