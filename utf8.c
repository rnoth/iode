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
 * (signed long)[].
 *
 * If there are broken UTF-8 characters, the value of all broken bytes are
 * stored as negative for each byte.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"


/*
 * Returns the number of bytes of the first UTF-8 character of `char *str`:
 *
 * - 1 for ASCII;
 * - 2, 3, 4 for other UTF-8;
 * - 0 for malformed UTF-8.
 */
int
utf8_length(char *str)
{
	int n = 1;

	/* check if char is UTF-8 or ASCII */
	if (str[0] & 1 << 7) {

		/* first byte give the number of continuation bytes */
		for (n = 1; (str[0] & 1 << (7 - n)); n++) {

			/* check formatting of continuation byte */
			if (n > 4 || !(str[n] & 1 << 7 && ~str[n] & 1 << 6)) {
				return 0;
			}
		}
	}

	return n;
}


/*
 * Convert an UTF-8 string into an array of long code points, one item per
 * long, with bytes not fitting the UTF-8 format stored as negative
 * numbers.
 */
void
str_to_runes(signed long text[], char *str)
{
	int i = 0, t = 0, k = 0;
	int n = 0;

	for (t = 0; str[i] && i < MAX_LINE_SIZE - 1; t++, i += n ? n : 1) {
		n = utf8_length(&str[i]);

		/* UTF-8 */
		if (n > 1) {
			text[t] = (str[i] % (1 << (7 - n))) << (n - 1) * 6;

			for (k = 1; str[i + k] && k < n; k++)
				text[t] |= (str[i + k] % 1 << 6) << 6 * (n - k);

		/* ASCII */
		} else if (n > 0) {
			text[t] = str[i];

		/* malformed UTF-8 bytes */
		} else if (n == 0) {
			text[t] = -str[i];
		}

		fprintf(stdout, "(%ld)", text[t]);
	}
}


void
runes_to_str()
{
	
}
