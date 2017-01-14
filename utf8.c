/*
 * ASCII all have a leading '0' byte:
 *
 *  0xxxxxxx
 *
 * UTF-8(7) have one leading '1' and as many following '1' as there are
 * continuation bytes (with leading '1' and '0').
 *
 *  110xxxxx 10xxxxxx
 *  1110xxxx 10xxxxxx 10xxxxxx
 *  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
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
utf8_byte_count(char *str)
{
	int n = 1;

	/* check if multibyte */
	if (str[0] & 1 << 7)

		/* get the number of continuation bytes */
		for (n = 1; (str[0] & 1 << (7 - n)); n++)

			/* check formatting */
			if (n > 7 || !(str[n] & 1 << 7 && ~str[n] & 1 << 6))
				return 0;

	return n;
}


/*
 * Return the number of bytes required to draw a rune
 */
int
utf8_required_bytes(long rune)
{
	int n = 0;

	while (n < 8 && rune >= 1 << 7 && rune >= 1 << ((6 - n) + (6 * n)))
		n++;

	return n + 1;
}


/*
 * Set `rune` the first rune of `str` and return a pointer to the beginning of
 * the next rune in `str`.
 */
char *
utf8_decode(long *rune, char str[])
{
	int i, n = utf8_byte_count(str);

	/* malformed byte sequence */
	if (n == 0) {
		*rune = -((unsigned char) str[0]);
		return &str[1];

	/* single byte sequence */
	} else if (n == 1) {
		*rune = str[0];
		return &str[1];
	}

	/* multibyte sequence */
	*rune = (unsigned char) str[0] % (1 << (7 - n));

	for (i = 1; i < n; i++)
		*rune = *rune << 6 | (unsigned char) str[i] % (1 << 6);

	if (utf8_required_bytes(*rune) != n) {
		*rune = -((unsigned char) str[0]);
		return &str[1];
	}

	return &str[n];
}


/*
 * Set `str` to the UTF-8 representation of `rune` and return its length.
 */
size_t
utf8_encode(char *str, long rune)
{
	int i, n = utf8_required_bytes(rune);

	if (rune < 1 << 7) {
		str[0] = (rune < 0) ? -rune : rune;
		str[1] = '\0';
		return 1;
	}

	str[n] = '\0';
	str[0] = 0;

	for (i = n - 1; i > 0; i--) {
		str[0] |= 1 << (7 - i);
		str[i] = (rune % (1 << 6)) | (1 << 7);
		rune = rune >> 6;
	}

	str[0] |= rune | 1 << 7;

	return n;
}
