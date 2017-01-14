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

	line->length = utf8_decode(&line->text, str, length);
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
 * Read a line of at most `size` chars from `file` into `str` and return its
 * length.  Unlike fgets, it can also read null character
 */
size_t
read_line(char **str, FILE *file)
{
	size_t i = 0, n = 1 << 7;

	*str = malloc(sizeof(char) * n);
	while (((*str)[i] = fgetc(file)) != EOF && (*str)[i] != '\n') {
		if (i >= n - 16) {
			n <<= 1;
			if ((*str = realloc(*str, n)) == NULL)
				die("realloc");
		}

		i++;
	}

	return i;
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

	/* read lines from `file` */
	for (n_total = 0; !feof(file) && !ferror(file); n_total++) {
		char *str;
		size_t i = read_line(&str, file);

		l_current = new_line(str, i);

		link_lines(l_last, l_current);

		l_last = l_current;
		l_first = l_first ? l_first : l_current;

		free(str);
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

		free(line->text);
		line->text = NULL;

		free(line);
		line = NULL;
	}
}
