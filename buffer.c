#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Allocates and initialize a new line with the string content.
 */
Line *
new_line(char *s)
{
	Line *line = malloc(sizeof(Line));

	line->text = malloc((strlen(s) + 1) * sizeof(char));
	strcpy(line->text, s);
	if (line->text[0] && line->text[strlen(line->text) - 1] == '\n')
		line->text[strlen(line->text) - 1] = '\0';

	line->next = line->prev = NULL;

	return line;
}


void
link_lines(Line *prev, Line *next)
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
	extern Line *l_current, *l_top, *l_first, *l_last;
	extern int n_total, n_top;
	extern char *filename;
	extern FILE *file;

	char s[MAX_LINE_SIZE];

	/* init empty buffer */
	l_first = l_last = l_current = l_top = NULL;
	n_total = n_top = 0;
	filename = name;

	/* open file or stdin */
	if (!filename) {
		file = stdin;
	} else if (!(file = fopen(filename, "r"))) {
		die("fopen");
	}

	for (n_total = 0; fgets(s, MAX_LINE_SIZE, file); n_total++) {
		l_current = new_line(s);
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
free_buffer(Line *line)
{
	Line *next = NULL;

	for (; line; line = next) {
		next = line->next;

		free(line->text);
		line->text = NULL;

		free(line);
		line = NULL;
	}
}
