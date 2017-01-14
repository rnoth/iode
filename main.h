/* constants */

#define PROGRAM_NAME  "iode"
#define MAX_LINE_SIZE 2048
#define MAX_KEYS      8
#define FLAGS "abcdefghijklmnopqrstuvwxy"


/* macros */

#define LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY)[0])
#define MIN(X, Y)     (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y)     (((X) > (Y)) ? (X) : (Y))
#define CONTROL(C)    (char) (C ^ 0x40)
#define ISASCII(C)    ((unsigned char) C <  0x80)


/* enums */

enum { PAGER = EXIT_FAILURE + 1, EDIT, SEARCH, INSERT, REPLACE, LAST };


/* structs */

struct line {
	long text[MAX_LINE_SIZE];
	size_t length;
	struct line *prev, *next;
};

struct cursor {
	int l, c;  /* cursor position in line and columns */
};


/* variables */

/* mode state */
char input[MAX_LINE_SIZE];
int flags[128];  /* command line and interactive options */
int multiplier, mode;

/* terminal */
int tty_fd;
int rows, cols;

/* buffer */
struct line *l_top, *l_current, *l_selected, *l_first, *l_last;
int n_total, n_top,  n_current,  n_selected, n_col_current, n_col_selected;
char *filename;
FILE *file;


/* functions */

/* main */
void die(const char *);
int  set_flag(char, int);

/* buffer */
struct line * new_line(char *s, size_t);
void read_buffer(char *filename);
void free_buffer(struct line *);

/* utf8 */
size_t utf8_encode(char [], long);
char * utf8_decode(long *, char []);

/* draw */
void draw_line(struct line *, int);
void update_line(struct line *, int);
void update_status_line(void);
void update_cursor(void);
void update_terminal_size(void);
void draw_screen(void);
void scroll_up(void);
void scroll_down(void);
void cursor_follow_screen();
void screen_focus_cursor();

/* input */
int  get_input(void);

/* edit */
void cursor_up(void);
void cursor_down(void);

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
