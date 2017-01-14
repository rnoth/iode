#include <stdio.h>
#include <stdlib.h>

int
main()
{
	char *first, *str = malloc(4000 * sizeof (char));

	fgets(str, 4000, stdin);

	first = str;

	str = &str[5];

	printf("%ld\n", str - first);

	free(first);

	return 0;
}
