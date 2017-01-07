/*
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Fill rune[4] with the next rune among string, and returns a pointer to the
 * beginning of the next rune.
 */
char *
str_to_rune(char rune[4], char *str)
{
	int i, n = 1;  /* number chars in the rune */

	rune[0] = rune[1] = rune[2] = rune[3] = '\0';

	/* check if char is UTF-8 or ASCII */
	if (str[0] & 1 << 7) {

		/* first byte give the number of continuation bytes */
		for (n = 1; (str[0] & 1 << (7 - n)); n++) {

			/* check formatting of continuation byte */
			if (n > 4 || !(str[n] & 1 << 7 && ~str[n] & 1 << 6)) {
				n = 1;
				break;
			}
		}
	}

	/* fill rune with byte(s) */
	for (i = 0; i < n ; i++)
		rune[i] = str[i];

	return &str[n];
}


/*
 * Return a pointer to an array of runes: array of up to 4 chars max
 * corresponding to the UTF-8 or ASCII character, or a single char if is
 * none of these both.
 *
 * Convert continuous feed of bytes into an array of runes.
 */
void
str_to_runes(char text[][4], char *str)
{
	int i;

	for (i = 0; str[0] && i < MAX_LINE_SIZE - 1; i++)
		str = str_to_rune(text[i], str);
}


void
runes_to_str(char text[][4])
{
	char *str = "";
	int i;

	for (i = 0; text[i][0]; i++)
		strncat(str, text[i], 4);
}
