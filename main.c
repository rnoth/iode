#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

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
	int tty_fd = open("/dev/tty", O_RDWR);
	/* fopen */

	Buffer *buffer = file_read(stdin);

	draw_lines(buffer->first, 10, tty_fd);

	buffer_free(buffer);

	/* fclose */

	return EXIT_SUCCESS;
}
