#include <stdlib.h>
#include <stdio.h>

#include "main.h"


/*
 * read the file in a doubly linked list of lines.
 */
Buffer *
read_file(void)
{
	/* fill buffer with string */
	Buffer *buffer = malloc(sizeof(Buffer));
	Line   *prev   = NULL;
	FILE   *fp     = stdin;
	char    s[MAX_LINE_SIZE];

	if (!fp)
		die("Can not open file for reading.");

	/* init empty buffer */
	buffer->first = buffer->last = buffer->current = NULL;
	buffer->total = 0;

	/* doubly linked list */
	while (fgets(s, MAX_LINE_SIZE, fp)) {
		buffer->last = malloc(sizeof(Line));
		buffer->last->text = s;
		buffer->last->prev = prev;
		buffer->last->next = NULL;

		if (prev) {
			buffer->last->prev->next = buffer->last;
		} else {
			buffer->first = buffer->last;
		}

		prev = buffer->last;
	}

	return buffer;
}


/*
 * Free the buffer, also recursing the doubly linked list.
 */
void
free_buffer(Buffer *buffer)
{
	Line *next = buffer->first;

	while (buffer->first) {
		next = buffer->first->next;

		buffer->first->text = NULL;
		free(buffer->first->text);

		buffer->first = NULL;
		free(buffer->first);

		buffer->first = next;
	}

	free(buffer);
}
