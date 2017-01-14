#include <stdio.h>
#include <stdlib.h>

int
main()
{
	int i;

	for (i = 0x80; i <= 0xff; i++) {
		putchar(0xc0 | i >> 6);
		putchar(i % (1 << 6) | 0x80);

		if (i == 0xa0)
			putchar('\n');
		if (i < 0xff)
			putchar('|');
	}

	return 0;
}
