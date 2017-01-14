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
new_line(char *str, size_t length)
{
	struct line *line = malloc(sizeof(struct line));

	if (length > 0 && str[length - 1] == '\n')
		length--;

	str[length] = '\0';

	for (line->length = 0; length > 0; line->length++, length--)
		str = utf8_decode(&line->text[line->length], str);

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

	for (n_total = 0; !feof(file) && !ferror(file); n_total++) {
		char str[MAX_LINE_SIZE];
		size_t i = 0;

		/* read a new line accepting null characters */
		while (i < MAX_LINE_SIZE && (str[i] = fgetc(file)) != EOF) {
			if (str[i] == '\n')
				break;
			i++;
		}

		/* create a new line and append it to the buffer */
		l_current = new_line(str, i);
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
