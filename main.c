#include <stdlib.h>
#include <stdio.h>

#include "main.h"


void
die(const char *message)
{
	perror(message);
}

int
main(int argc, char *arcgv[])
{
	Buffer *buffer = read_file();
	free_buffer(buffer);

	return 0;
}
