#define PROGRAM_NAME "triode"
#define MAX_LINE_SIZE           2048
#define MAX_KEY_SEQUENCE_LENGTH 8

/* thanks to martanne@brain-dump.org, this is neat */
#define CONTROL(c)    (c ^ 0x40)
#define ISCONTROL(c)  ((unsigned char) c <  0x1f)
#define ISASCII(c)    ((unsigned char) c <  0x80)
#define ISUTF8(c)     ((unsigned char) c >= 0x80)
#define MIN(X, Y)     (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)     (((X) > (Y)) ? (X) : (Y))


enum { CONTINUE = 2, PAGER_MODE, NORMAL_MODE, INSERT_MODE, REPLACE_MODE };


/*
 * Doubly linked list of lines.
 */
typedef struct Line {
	char *text;         /* content of the line */

	struct Line *prev, *next;
} Line;


typedef struct Buffer {
	int total;           /* total number of lines */
	int top_l;           /* number of top line displayed on the screen */
	int l, c;            /* cursor position in line and columns */

	Line *first, *last;
	Line *current;
	Line *top;           /* line at the top of the screen */

	char operators[MAX_LINE_SIZE];
} Buffer;


typedef struct Arg {
	Buffer *b;
	
	int m;     /* count before commands */
	int r, c;  /* rows columns */
} Arg;


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
void     draw_line(Line *, int, int);
void     draw_status_line(Buffer *, int);
void     update_status_line(Buffer *, int, int);
void     draw_screen(Buffer *, int, int);
void     scroll_up(Buffer *, int, int);
void     scroll_down(Buffer *, int, int);


/* input */

char     input_modifier(Arg *, char, FILE *);
int      input(FILE *, int, Buffer *);


/* actions */

int      a_quit();
int      a_redraw(Arg *);
int      a_jump_begin(Arg *);
int      a_jump_end(Arg *);
int      a_half_page_up(Arg *);
int      a_half_page_down(Arg *);
int      a_page_up(Arg *);
int      a_page_down(Arg *);
int      a_scroll_up(Arg *);
int      a_scroll_down(Arg *);
int      a_increment_multiplier(Arg *);
