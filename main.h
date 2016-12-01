#define MAX_LINE_SIZE 2048


/*
 * Doubly linked list of lines.
 */
typedef struct Line {
	char *text;  /* content of the line */

	struct Line *next;  /* doubly linked list structure */
	struct Line *prev;
} Line;


typedef struct Buffer {
	int total;  /* total number of lines */

	Line *first;
	Line *last;
	Line *current;  /* for walking the doubly linked list faster */

	int l, c;
} Buffer;


/* main */

void     die(const char *);


/* buffer */

Buffer * file_read(FILE *);
Line   * line_new(char *s);
void     line_add_end(Buffer *, Line *line);
void     buffer_free(Buffer *);
void     buffer_print(Buffer *);

/* draw */
