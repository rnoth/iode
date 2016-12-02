#define MAX_LINE_SIZE 2048


/* thanks to martanne@brain-dump.org, this is pretty neat */
#define ISASCII(c) ((unsigned char) c <  0x80)
#define ISUTF8(c)  ((unsigned char) c >= 0x80)


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

void     draw_lines(Line *, int, int);
void     draw_line(Line *, int);
int      draw_char(char **, char **, int);
