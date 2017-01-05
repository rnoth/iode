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

struct line {
	char *text;
	struct line *prev, *next;
};

struct cursor {
	int l, c;  /* cursor position in line and columns */
};

struct keybinding {
	char keys[MAX_KEYS];
	void (*action)(void);
};



/* variables */

/* mode state */
char operators[MAX_LINE_SIZE];
int flags[128];  /* command line and interactive options */
int multiplier, mode;

/* terminal */
int tty_fd;
int rows, cols;

/* buffer */
struct line *l_current, *l_top, *l_first, *l_last;
int n_total, n_current, n_top;
char *filename;
FILE *file;


/* functions */

/* main */
void die(const char *);
int  set_flag(char, int);

/* buffer */
struct line * new_line(char *s);
void read_buffer(char *filename);
void free_buffer(struct line *);

/* draw */
void draw_line(struct line *, int);
void draw_screen(void);
void update_status_line(void);
void update_terminal_size(void);
void scroll_up(void);
void scroll_down(void);
void update_cursor(void);

/* input */
int  input(void);

/* edit */
void cursor_up();
void cursor_down();

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
void a_cursor_up(void);
void a_cursor_down(void);
void a_editor(void);
