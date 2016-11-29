#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * read the file in a doubly linked list of lines.
 */
Buffer *
read_file(void)
{
	/* fill buffer with string */
	Buffer *buffer = malloc(sizeof(Buffer));
	Line   *line   = NULL;
	FILE   *fp     = stdin;
	char    s[MAX_LINE_SIZE];

	if (!fp)
		die("Can not open file for reading.");

	/* init empty buffer */
	buffer->first = buffer->last = buffer->current = NULL;
	buffer->total = 0;

	/* doubly linked list */
	while (fgets(s, MAX_LINE_SIZE, fp)) {
		line = malloc(sizeof(Line));
		line->prev = buffer->last;
		line->next = NULL;

		if (buffer->last) {
			buffer->last->next = line;
		} else {
			buffer->first = line;
		}

		/* set line content without trailing newline */
		line->text = malloc(sizeof(s));
		strcpy(line->text, s);
		line->text[strlen(line->text) - 1] = '\0';

		buffer->last = line;
	}

	return buffer;
}


/*
 * Free the buffer, also recursing the doubly linked list.
 */
void
free_buffer(Buffer *buffer)
{
	while (buffer->first) {

		buffer->first->text = NULL;
		free(buffer->first->text);

		free(buffer->first);

		buffer->first = buffer->first->next;
	}

	free(buffer);
}
