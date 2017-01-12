/*
 * Buffer lifecycle and modification function.
 *
 * The unique iode buffer is a doubly linked list of `struct lines *`, from
 * `l_first` to `l_last`, with `n_total` lines.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Allocates and initialize a new line with the string content.
 */
struct line *
new_line(char *str)
{
	struct line *line = malloc(sizeof(struct line));
	int i;

	for (i = 0; i < MAX_LINE_SIZE; i++)
		line->text[i] = 0;

	if (str[0] && str[strlen(str) - 1] == '\n')
		str[strlen(str) - 1] = '\0';

	for (i = 0; i < MAX_LINE_SIZE; i++)
		str = utf8_decode(&line->text[i], str);

	line->next = line->prev = NULL;

	return line;
}


void
link_lines(struct line *prev, struct line *next)
{
	if (prev)
		prev->next = next;
	if (next)
		next->prev = prev;
}


/*
 * Read the file in a doubly linked list of lines.
 */
void
read_buffer(char* name)
{
	extern struct line *l_current, *l_top, *l_first, *l_last;
	extern int n_total, n_current, n_top;
	extern char *filename;
	extern FILE *file;

	char str[MAX_LINE_SIZE];

	/* init empty buffer */
	l_current = l_top = l_first = l_last = NULL;
	n_current = n_top = 1;
	filename = name;

	/* open file or stdin */
	if (!filename) {
		file = stdin;
	} else if (!(file = fopen(filename, "r"))) {
		die("fopen");
	}

	for (n_total = 0; fgets(str, MAX_LINE_SIZE, file); n_total++) {
		l_current = new_line(str);
		link_lines(l_last, l_current);
		l_last = l_current;
		l_first = l_first ? l_first : l_current;
	}

	l_top = l_current = l_first;
}


/*
 * Free the buffer and its lines, starting from the first one.
 */
void
free_buffer(struct line *line)
{
	struct line *next = NULL;

	for (; line; line = next) {
		next = line->next;

		free(line);
		line = NULL;
	}
}
