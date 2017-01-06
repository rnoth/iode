#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Return a pointer to an array of runes: array of up to 4 chars max
 * corresponding to the UTF-8 or ASCII character, or a single char if is
 * none of these both.
 *
 * ASCII all have a leading '0' byte:
 *
 *  0xxxxxxx
 *
 * UTF-8 have one leading '1' and as many following '1' as there are
 * continuation bytes which have a leading '1' and a following '0':
 *
 *  110xxxxx 10xxxxxx
 *
 *  1110xxxx 10xxxxxx 10xxxxxx
 *
 *  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * There is up to 3 continuation bytes -- up to 4 bytes per runes.
 *
 * This function will convert continuous feed of bytes into an array of
 * runes.
 *
 * Every array element has 4 bytes, with all empty, but those from the
 * parsed UTF-8 string:
 *
 * ASCII:
 *
 *  0xxxxxxx 00000000 00000000 00000000
 *
 * UTF-8:
 *
 *  110xxxxx 10xxxxxx 00000000 00000000
 *  1110xxxx 10xxxxxx 10xxxxxx 00000000
 *  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * The others -- 'broken' runes -- get split as single characters:
 *
 * Runes with broken continuation byte:
 *
 *  11xxxxxx 00000000 00000000 00000000  00xxxxxx 00000000 00000000 00000000
 *  11xxxxxx 00000000 00000000 00000000  11xxxxxx 00000000 00000000 00000000
 *
 * Runes with leading byte with more than 4 or less than leading 2 bits:
 *
 *  10xxxxxx 00000000 00000000 00000000
 *  111111xx 00000000 00000000 00000000
 *
 * If there is any other kind output from this function, this may be a bug.
 */
void
string_to_runes(char text[][4], char *string)
{
	int r = 0;  /* index in text[][] of rune[] */
	int s = 0;  /* index in string[] */
	int n = 1;  /* number of chars in one rune */
	int i = 0;

	for (r = 0; string[s] && r < MAX_LINE_SIZE - 1; r++) {
		text[r][0] = text[r][1] = text[r][2] = text[r][3] = '\0';

		/* check if char is UTF-8 or ASCII */
		if (string[s] & 1 << 7) {

			/* first byte give the number of continuation bytes */
			for (n = 1; (string[s] & 1 << (7 - n)); n++) {

				/* check formatting of continuation byte */
				if (
					!( string[s + n] & 1 << 7) ||
					!(~string[s + n] & 1 << 6) ||
					n > 4
				) {
					n = 1;
					break;
				}
			}
		}

		/* fill rune with byte(s) */
		for (i = 0; i < n ; i++, s++)
			text[r][i] = string[s];
	}
}



/*
 * Allocates and initialize a new line with the string content.
 */
struct line *
new_line(char *s)
{
	struct line *line = malloc(sizeof(struct line));

	if (s[0] && s[strlen(s) - 1] == '\n')
		s[strlen(s) - 1] = '\0';

	string_to_runes(line->text, s);

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
 * Return the string contained in rune.
 */
void
runes_to_string(char *string, char text[][4])
{
	int i;
	for (i = 0; text[i][0]; i++)
		strncat(string, text[i], 4);
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
	char text[MAX_LINE_SIZE][4];

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
		string_to_runes(text, s);
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
