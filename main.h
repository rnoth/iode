/* constants */

#define PROGRAM_NAME            "iode"
#define MAX_LINE_SIZE           2048
#define MAX_KEY_SEQUENCE_LENGTH 8
#define OPTIONS                 "abcdefghijklmnopqrstuvwxy" \
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"


/* macros */

#define CONTROL(c)   (c ^ 0x40)
#define ISASCII(c)   ((unsigned char) c <  0x80)
#define ISUTF8(c)    ((unsigned char) c >= 0x80)
#define MIN(X, Y)    (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)    (((X) > (Y)) ? (X) : (Y))


/* enums */

enum { CONTINUE = 2, PAGER, EDIT, SEARCH, INSERT, REPLACE };


/* datatypes */

typedef struct Line {
	char *text;
	struct Line *prev, *next;
} Line;

typedef struct Buffer {
	Line *first, *last;
	char *filename;
	FILE *file;
} Buffer;

typedef struct Cursor {
	int l, c;  /* cursor position in line and columns */
} Cursor;

typedef char mode;


/* variables */

int options[128];
Buffer *buffer;
Line *current, *top;
char operators[MAX_LINE_SIZE];
int n_total, n_top;  /* line numbers */
struct winsize winsize;


/* functions */

/* main */
void usage(void);
void die(const char *);
int  set_option(char);
void raw_terminal(int);
void unraw_terminal(int);

/* buffer */
Buffer * buffer_read(char *filename);
Line   * line_new(char *s);
void     line_add_end(Buffer *, Line *line);
void     buffer_free(Buffer *);
void     buffer_print(Buffer *);

/* draw */
int  draw_char(char **, char **, int, int);
void draw_line(Line *, int, int);
void draw_empty_line(void);
void draw_status_line(Buffer *, int);
void update_status_line(Buffer *, int, int);
void draw_screen(Buffer *, int, int);
void scroll_up(Buffer *, int, int);
void scroll_down(Buffer *, int, int);

/* input */
char input_modifier(char, FILE *);
int  input(FILE *, int, Buffer *);

/* actions */
mode a_quit();
mode a_redraw(void);
mode a_jump_begin(void);
mode a_jump_end(void);
mode a_half_page_up(void);
mode a_half_page_down(void);
mode a_page_up(void);
mode a_page_down(void);
mode a_scroll_up(void);
mode a_scroll_down(void);
mode a_increment_multiplier(void);
mode a_editor(void);
