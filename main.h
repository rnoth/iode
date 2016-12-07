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


enum { CONTINUE = 2, PAGER, EDIT, SEARCH, INSERT, REPLACE };


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
	int mode;

	Line *first, *last;
	Line *current;
	Line *top;           /* line at the top of the screen */

	char  operators[MAX_LINE_SIZE];

	char *filename;
	FILE *file;
} Buffer;


typedef struct Arg {
	Buffer *b;
	
	int m;     /* count before commands */
	int r, c;  /* rows columns */
} Arg;


/* main */

void           die(const char *);
struct termios set_terminal(int);


/* buffer */

Buffer * buffer_read(char *filename);
Line   * line_new(char *s);
void     line_add_end(Buffer *, Line *line);
void     buffer_free(Buffer *);
void     buffer_print(Buffer *);


/* draw */

int      draw_char(char **, char **, int, int);
void     draw_line(Line *, int, int);
void     draw_empty_line(void);
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
int      a_editor(Arg *);
