#define MAX_LINE_SIZE 2048

#define CONTINUE 2

/* thanks to martanne@brain-dump.org, this is pretty neat */
#define ISCONTROL(c)  ((unsigned char) c <  0x1f)
#define ISASCII(c)    ((unsigned char) c <  0x80)
#define ISUTF8(c)     ((unsigned char) c >= 0x80)
#define CONTROL(c)    (c ^ 0x40)
#define MIN(X, Y)     (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)     (((X) > (Y)) ? (X) : (Y))


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
	Line *top;      /* line at the top of the screen */

	int top_l;      /* number of top line displayed on the screen */
	int l, c;       /* cursor position in line and columns through file */
} Buffer;


/* main */

void     die(const char *);


/* buffer */

Buffer * buffer_read(FILE *);
Line   * line_new(char *s);
void     line_add_end(Buffer *, Line *line);
void     buffer_free(Buffer *);
void     buffer_print(Buffer *);


/* draw */

int      draw_char(char **, char **, int);
void     draw_line(Line *, int);
void     draw_status_line(Buffer *, int);
void     draw_screen(Buffer *, int, int);
void     scroll_up(Buffer *, int, int);
void     scroll_down(Buffer *, int, int);


/* input */
int      input_key(FILE *, int, Buffer *);
int      input_get(FILE *, int, Buffer *);
