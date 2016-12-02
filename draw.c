#include <stdio.h>
#include <sys/ioctl.h>

#include "main.h"


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


/*
 * Draw a full line up to the width of the screen.
 */
void
draw_line(Line *line, int cols)
{
	int i = 0, shift = 0;

	while (line->text[i] && cols - shift > 0)
		i = draw_char(line->text, i, &shift);

	fputc('\n', stdout);
}


/*
 * Print a single ASCII, UTF-8, or non-printable char, only if its width does
 * not overflow cols.
 *
 * Also converts non-printable chars to a visible way.
 *
 * Return the next index in the string, and increment shift by the width the
 * character took;
 */
int
draw_char(char text[], int i, int *shift)
{
	*shift += text[i] == '\t' ? 8 - (*shift % 8) : 1;

	if (ASCII(text[i])) {
		fputc(text[i++], stdout);
	} else {
		while (text[i] && UTF8(text[i]))
			fputc(text[i++], stdout);
	}

	return i;
}
