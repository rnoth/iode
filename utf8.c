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
 * The whole character value is retreived into an 'x' and stored into a
 * (long)[].
 *
 * If there are broken UTF-8 characters, the value of all broken bytes are
 * stored as negative for each byte.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Returns the number of bytes for the first rune of `str`: 1, 2, 3, 4 or 0 for
 * malformed UTF-8.
 */
int
utf8_length(char *str)
{
	int n = 1;

	/* check if multibyte */
	if (str[0] & 1 << 7)

		/* get the number of continuation bytes */
		for (n = 1; (str[0] & 1 << (7 - n)); n++)

			/* check formatting */
			if (n > 4 || !(str[n] & 1 << 7 && ~str[n] & 1 << 6))
				return 0;

	return n;
}


/*
 * Set `rune` the first rune of `str` and return a pointer to the beginning of
 * the next rune in `str`.
 */
char *
next_rune(long *rune, char str[])
{
	int i, n = utf8_length(str);

	/* UTF-8 */
	if (n > 1) {
		*rune = (unsigned char) str[0] % (1 << (7 - n)) << 6 * (n - 1);

		for (i = 1; i < n; i++)
			*rune |= (unsigned char)
				str[i] % (1 << 6) << 6 * (n - i - 1);

	/* ASCII */
	} else if (n > 0) {
		*rune = str[0];

	/* malformed UTF-8 byte */
	} else if (n == 0) {
		*rune = -str[0];
	}

	return &str[n ? n : 1];
}

/*
 * Convert an UTF-8 string into an array of long code points, one item per
 * long, with bytes not fitting the UTF-8 format stored as negative
 * numbers.
 */
void
str_to_runes(long text[], char str[])
{
	int i = 0;

	for (i = 0; str[0] && i < MAX_LINE_SIZE - 1; i++)
		str = next_rune(&text[i], str);
}


void
runes_to_str()
{
	
}
