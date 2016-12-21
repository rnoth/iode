#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


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

	while (fgets(s, MAX_LINE_SIZE, file))
		line_add_end(line_new(s));

	l_top = l_current = l_first;
}


/*
 * Allocates and initialize a new line with the string content.
 */
Line *
line_new(char *s)
{
	Line *line = malloc(sizeof(Line));

	line->text = malloc((strlen(s) + 1) * sizeof(char));
	strcpy(line->text, s);
	if (line->text[strlen(line->text) - 1] == '\n')
		line->text[strlen(line->text) - 1] = '\0';

	return line;
}


/*
 * Add a line to the end of a doubly linked list buffer, which may be empty.
 */
void
line_add_end(Line *line)
{
	extern Line *l_last, *l_first;
	extern int n_total;

	line->prev = l_last;
	line->next = NULL;

	if (!n_total) {
		l_first = line;
	} else {
		l_last->next = line;
	}
	l_last = line;

	n_total++;
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
