/* constants */

#define PROGRAM_NAME            "iode"
#define MAX_LINE_SIZE           2048
#define MAX_KEYS                8
#define FLAGS "abcdefghijklmnopqrstuvwxy"


/* macros */

#define LENGTH(ARRAY)  (sizeof(ARRAY) / sizeof(ARRAY)[0])
#define MIN(X, Y)      (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)      (((X) > (Y)) ? (X) : (Y))
#define CONTROL(c) (char) (c ^ 0x40)
#define ISASCII(c) ((unsigned char) c <  0x80)
#define ISUTF8(c)  ((unsigned char) c >= 0x80)


/* enums */

enum { PAGER = EXIT_FAILURE + 1, EDIT, SEARCH, INSERT, REPLACE };


/* datatypes */

typedef struct Line {
	char *text;
	struct Line *prev, *next;
} Line;

typedef struct Cursor {
	int l, c;  /* cursor position in line and columns */
} Cursor;

typedef struct keybinding {
	char keys[MAX_KEYS];
	void (*action)(void);
} keybinding;



/* variables */

char operators[MAX_LINE_SIZE];
int flags[128];  /* command line and interactive options */
int multiplier;

/* terminal */
int tty_fd;
int rows, cols;

/* buffer */
Line *l_current, *l_top, *l_first, *l_last;
int n_total, n_top;
char *filename;
FILE *file;


/* functions */

/* main */
void die(const char *);
int  set_flag(char, int);

/* buffer */
void     read_buffer(char *filename);
Line   * new_line(char *s);
void     free_buffer(Line *);

/* draw */
void draw_line(Line *, int);
void draw_screen(void);
void update_status_line(void);
void update_terminal_size(void);
void scroll_up(void);
void scroll_down(void);

/* input */
int  input();

/* actions */
void a_quit(void);
void a_redraw(void);
void a_jump_begin(void);
void a_jump_end(void);
void a_half_page_up(void);
void a_half_page_down(void);
void a_page_up(void);
void a_page_down(void);
void a_scroll_up(void);
void a_scroll_down(void);
void a_editor(void);
