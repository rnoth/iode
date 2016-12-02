#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/ioctl.h>

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
			c[0] = t[1]; c[1] = '\0';
			width = 1;

		} else if (t[0] == '\t') {
			c[0] = '\t'; c[1] = '\0';
			width = 8 - (col % 8);

		} else if (t[0] < 0x1f) {
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

	/* draw chars until the screen is filled or end of string */
	for (col = 0; text[0] && cols - col > 0;) {
		col += draw_char(&c, &text, col);
		/* now `c` is set and `text` points to the next char */
		fputs(c, stdout);

		/* not enough space to fit next char onscreen */
		if (cols - col < 2 && text[0] && text[1]) {
			fputs("\033[1;31m>\033[0m", stdout);
			break;
		}
	}

	fputc('\n', stdout);
	
	free(c);
}


/*
 * Draw lines from line up to specified count.
 */
void
draw_lines(Line *line, int rows, int tty_fd)
{
	struct winsize w;
	
	if (ioctl(tty_fd, TIOCGWINSZ, &w) > 0)
		die("ioctl");

	for (; line && rows > 0; line = line->next, rows--)
		draw_line(line, w.ws_col);
}
