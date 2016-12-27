/* constants */

#define PROGRAM_NAME            "iode"
#define MAX_LINE_SIZE           2048
#define MAX_KEY_SEQUENCE_LENGTH 8
#define OPTIONS "abcdefghijklmnopqrstuvwxy"


/* macros */

#define CONTROL(c)   (c ^ 0x40)
#define ISASCII(c)   ((unsigned char) c <  0x80)
#define ISUTF8(c)    ((unsigned char) c >= 0x80)
#define MIN(X, Y)    (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)    (((X) > (Y)) ? (X) : (Y))


/* enums */

enum { PAGER = EXIT_FAILURE + 1, EDIT, SEARCH, INSERT, REPLACE };
enum { RAW, RESET };


/* datatypes */

typedef struct Line {
	char *text;
	struct Line *prev, *next;
} Line;

typedef struct Cursor {
	int l, c;  /* cursor position in line and columns */
} Cursor;

typedef char mode;


/* variables */

char operators[MAX_LINE_SIZE];
int options[128];
int tty_fd;
int rows, cols;

/* buffer */
Line *l_current, *l_top, *l_first, *l_last;
int n_total, n_top;
char *filename;
FILE *file;


/* functions */

/* main */
void usage(void);
void die(const char *);
int  set_option(char);
void set_terminal(int);

/* buffer */
void     read_buffer(char *filename);
Line   * line_new(char *s);
void     line_add_end(Line *line);
void     free_buffer(Line *);

/* draw */
int  draw_char(char **, char **, int);
void draw_line(Line *, int);
void draw_empty_line(void);
void draw_status_line(void);
void update_status_line(void);
void draw_screen(void);
void update_terminal_size(void);
void scroll_up(void);
void scroll_down(void);

/* input */
char input_modifier(char);
int  input();

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
