#include <stdlib.h>
#include <stdio.h>

#include "main.h"


void
die(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}


int
main(int argc, char *arcgv[])
{
	/* fopen */

	Buffer *buffer = file_read(stdin);

	buffer_free(buffer);

	/* fclose */

	return EXIT_SUCCESS;
}
