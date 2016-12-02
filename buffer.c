#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * read the file in a doubly linked list of lines.
 */
Buffer *
file_read(FILE *fp)
{
	/* fill buffer with string */
	Buffer *buffer = malloc(sizeof(Buffer));
	char    s[MAX_LINE_SIZE];

	/* init empty buffer */
	buffer->first = buffer->last = buffer->current = NULL;
	buffer->total = 0;

	while (fgets(s, MAX_LINE_SIZE, fp))
		line_add_end(buffer, line_new(s));

	return buffer;
}


/*
 * Allocates and initialize a new line with the string content.
 */
Line *
line_new(char *s)
{
	Line *line = malloc(sizeof(Line));

	line->text = malloc(strlen(s) * sizeof(char));
	strcpy(line->text, s ? s : "");
	if (line->text[strlen(line->text) - 1] == '\n')
		line->text[strlen(line->text) - 1] = '\0';

	return line;
}


/*
 * Add a line to the end of a doubly linked list buffer, which may be empty.
 */
void
line_add_end(Buffer *buffer, Line *line)
{
	line->prev = buffer->last;
	line->next = NULL;

	if (!buffer->total) {
		buffer->first = line;
	} else {
		buffer->last->next = line;
	}
	buffer->last = line;

	buffer->total++;
}


/*
 * Insert a line before the specified line.
 */


/*
 * Free the buffer and its lines, starting from the first one.
 */
void
buffer_free(Buffer *buffer)
{
	Line *next = NULL;

	for (; buffer->first; buffer->first = next) {
		next = buffer->first->next;

		free(buffer->first->text);
		buffer->first->text = NULL;

		free(buffer->first);
		buffer->first = NULL;
	}

	free(buffer);
}
