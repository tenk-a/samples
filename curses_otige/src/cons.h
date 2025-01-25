// CONS: ncurses, pdcurses を用いたコンソール描画.
// 注意! このファイルはヘッダでなく 単なる include ファイル.

#if defined(USE_PDCURSES)
#include <curses.h>
#define cons_clear()            clear()
#else
#include <sys/time.h>
#include <ncurses.h>
#define cons_clear()            erase()
#endif

typedef unsigned long           cons_timer_t;
typedef uint8_t                 cons_bool_t;
typedef int                     cons_pos_t;
typedef uint8_t                 cons_col_t;
typedef uint16_t                cons_key_t;

#define CONS_TIMER_BASE         60U         // 1000
#define CONS_MSEC_TO_TIMER(ms)  (((ms) * CONS_TIMER_BASE) / 1000U)

#define CONS_KEY_ERR            0xffff
#define CONS_KEY_DOWN           KEY_DOWN
#define CONS_KEY_UP             KEY_UP
#define CONS_KEY_LEFT           KEY_LEFT
#define CONS_KEY_RIGHT          KEY_RIGHT
#define CONS_KEY_RETURN         0x0a
#define CONS_KEY_ESC            0x1B

#define CONS_COL_REV            0x10        // 文字色反転フラグ.
#define CONS_COL_LIGHT          0x8         // 文字色明るいフラグ.

#define cons_screenWidth()      _cons_screen_width
#define cons_screenHeight()     _cons_screen_height
#define cons_key()              _cons_cur_key
#define cons_timer()            _cons_cur_timer
#define cons_xycprintf(x,y,co,...) do { attron(COLOR_PAIR(co)); mvprintw((y),(x),__VA_ARGS__); } while (0)

static cons_pos_t               _cons_screen_width;
static cons_pos_t               _cons_screen_height;
static cons_key_t               _cons_cur_key;
static cons_timer_t             _cons_cur_timer;

/// ミリ秒取得.
static cons_timer_t _cons_getTimer(void) {
 #if defined __DJGPP__
    return (cons_timer_t)(uclock() * CONS_TIMER_BASE / UCLOCKS_PER_SEC);
 #elif defined(__DOS__) || defined(_WIN32)
    return (cons_timer_t)(clock() * CONS_TIMER_BASE / CLOCKS_PER_SEC);
 #else
    struct timeval tv = {0,0};
    gettimeofday(&tv, NULL);
    return (cons_timer_t)((tv.tv_sec * 1000U + (tv.tv_usec / 1000U)) * CONS_TIMER_BASE / 1000U);
 #endif
}

/// cons 初期化.
cons_bool_t  cons_init(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors() == FALSE) {
        endwin();
        return 0;
    }
    {   // 色設定.
        static int const cols[8] = { 0, COLOR_BLUE, COLOR_RED, COLOR_MAGENTA,
                        COLOR_GREEN, COLOR_CYAN, COLOR_YELLOW, COLOR_WHITE };
        int i;
        start_color();
        for (i = 1; i < 8; ++i) {
            init_pair(       i,   cols[i], COLOR_BLACK);    // 字.
            init_pair(     8|i, 8|cols[i], COLOR_BLACK);    // 明字.
            init_pair(0x10|  i, COLOR_BLACK,   cols[i]);    // 背景.
            init_pair(0x10|8|i, COLOR_BLACK, 8|cols[i]);    // 明背景.
        }
    }
    getmaxyx(stdscr, _cons_screen_height, _cons_screen_width);
    timeout(50);    // getch のタイムアウト時間(50ミリ秒)
    return 1;
}

/// cons 終了処理.
void cons_term(void) {
    endwin();
}

/// 毎フレームの最初に行う処理.
void cons_updateBegin(void) {
    _cons_cur_timer = (cons_timer_t)(_cons_getTimer());
    _cons_cur_key   = (cons_key_t)getch();
    getmaxyx(stdscr, _cons_screen_height, _cons_screen_width);
}

/// 毎フレームの最後に行う処理.
void cons_updateEnd(void) {
    refresh();
}
