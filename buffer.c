#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Allocates and initialize a new line with the string content.
 */
struct line *
new_line(char *s)
{
	struct line *line = malloc(sizeof(struct line));

	line->text = malloc((strlen(s) + 1) * sizeof(char));
	strcpy(line->text, s);
	if (line->text[0] && line->text[strlen(line->text) - 1] == '\n')
		line->text[strlen(line->text) - 1] = '\0';

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
 * Shift `text` pointer to next rune then return an array of 4 chars max
 * corresponding to the UTF-8 or ASCII character, or a single char if is none
 * of these both.
 *
 * ASCII all have a leading '0' byte:
 *
 *  0xxxxxxx
 *
 * UTF-8 have one leading '1' and as many following '1' as there are
 * continuation bytes which have a leading '1' and a following '0':
 *
 *  110xxxxx 10xxxxxx
 *  1110xxxx 10xxxxxx 10xxxxxx
 *  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * There is up to 3 continuation bytes -- up to 4 bytes per runes.
 */
char *
add_rune(char rune[], char text[])
{
	int i = 0, n = 1;  /* number of chars in rune */
	rune[0] = rune[1] = rune[2] = rune[3] = rune[4] = '\0';

	/* check if char is UTF-8 or ASCII */
	if (text[0] & 1 << 7) {

		/* count number of announced continuation bytes */
		for (n = 1; n <= 4 && (text[0] & 1 << (7 - n)); n++) {

			/* check formatting of continuation byte */
			if (!(text[n] & 1 << 7) || !(~text[n] & 1 << 6)) {
				n = 1;
				break;
			}
		}
	}

	/* fill rune with byte(s) */
	for (i = 0; i < n ; i++)
		rune[i] = text[i];

	return text + sizeof(char);
}


/*
 * Read a string in an array of runes up to `MAX_LINE_SIZE` characters.
 */
void
to_runes(char *s)
{
	char runes[MAX_LINE_SIZE][5];
	int i;

	for (i = 0; s[0] && i < MAX_LINE_SIZE - 1; i++) {
		s = add_rune(runes[i], s);
		fprintf(stderr, "%s", runes[i]);
	}

	runes[MAX_LINE_SIZE][0] = '\0';
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

	char s[MAX_LINE_SIZE];

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

	for (n_total = 0; fgets(s, MAX_LINE_SIZE, file); n_total++) {
		l_current = new_line(s);
		link_lines(l_last, l_current);
		l_last = l_current;
		l_first = l_first ? l_first : l_current;
		to_runes(s);
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
