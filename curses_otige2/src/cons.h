// CONS: ncurses, pdcurses を用いたコンソール描画.
// 注意! このファイルはヘッダでなく 単なる include ファイル.

#if defined(_WIN32)
 #include <windows.h>
 #include <curses.h>
 typedef unsigned __int64       cons_clock_t;
 #define cons_clear()           clear()
 #define CONS_CLOCK_PER_SEC     1000000LL   // マイクロ秒.
#elif defined(__DOS__)
 #include <curses.h>
 #define cons_clear()           clear()
 #if defined(__DJGPP__)
  typedef unsigned long long    cons_clock_t;
  #define CONS_CLOCK_PER_SEC    1000000LL   // マイクロ秒.
 #else
  typedef unsigned long         cons_clock_t;
  #define CONS_CLOCK_PER_SEC    1000        // ミリ秒.
 #endif
#else   // linux,unix
 #include <sys/time.h>
 #include <ncurses.h>
 typedef unsigned long long     cons_clock_t;
 #define cons_clear()           erase()
 #define CONS_CLOCK_PER_SEC     1000000LL   // マイクロ秒.
#endif
#include <time.h>
#define CONS_FPS                60

typedef int                     cons_pos_t;
typedef unsigned char           cons_col_t;
typedef unsigned short          cons_key_t;

#define CONS_MSEC_TO_CLOCK(ms)  (((ms) * CONS_CLOCK_PER_SEC) / 1000U)

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
#define cons_clock()            _cons_cur_clock
#define cons_xycprintf(x,y,co,...) do { attron(COLOR_PAIR(co)); mvprintw((y),(x),__VA_ARGS__); } while (0)

static cons_pos_t               _cons_screen_width;
static cons_pos_t               _cons_screen_height;
static cons_key_t               _cons_cur_key;
static cons_clock_t             _cons_cur_clock;
static cons_clock_t             _cons_fps_count;

/// マイクロ秒 or ミリ秒取得.
static cons_clock_t _cons_getClock(void) {
 #if defined(_WIN32)
    static unsigned __int64 per_sec = 0;
    unsigned __int64        count   = 0;
    if (!per_sec)
        QueryPerformanceFrequency((LARGE_INTEGER*)&per_sec);
    QueryPerformanceCounter((LARGE_INTEGER*)&count);
    return count * CONS_CLOCK_PER_SEC / per_sec;
 #elif defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (cons_clock_t)ts.tv_sec * 1000000ULL + (ts.tv_nsec / 1000ULL);
 #elif defined(__DJGPP__)
    return (cons_clock_t)(uclock() * CONS_CLOCK_PER_SEC / UCLOCKS_PER_SEC);
 #elif defined(__DOS__)
    return (cons_clock_t)(clock() * CONS_CLOCK_PER_SEC / CLOCKS_PER_SEC);
 #else
    struct timeval tv = {0,0};
    gettimeofday(&tv, NULL);
    return (cons_clock_t)((tv.tv_sec * 1000000ULL + tv.tv_usec) * CONS_CLOCK_PER_SEC / 1000000ULL);
 #endif
}

// sleep.
static void cons_clock_sleep(cons_clock_t count) {
 #if defined(_WIN32)
    Sleep(count * 1000 / CONS_CLOCK_PER_SEC);
 #elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L) || defined(__APPLE__)
    struct timespec ts;
    ts.tv_sec  = count / CONS_CLOCK_PER_SEC;
    count %= CONS_CLOCK_PER_SEC;
    ts.tv_nsec = (long)(count * (1000000000LL / CONS_CLOCK_PER_SEC));
    nanosleep(&ts, &ts);
 #elif !defined(__DOS__)
    usleep(count * 1000000LL / CONS_CLOCK_PER_SEC);
 #endif
}

/// cons 初期化.
int cons_init(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);    // getch での待ちを無しにする.
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
    _cons_cur_clock = _cons_getClock();
    return 1;
}

/// cons 終了処理.
void cons_term(void) {
    endwin();
}

/// 毎フレームの最初に行う処理.
void cons_updateBegin(void) {
    _cons_cur_clock = _cons_getClock();
    _cons_fps_count = _cons_cur_clock * CONS_FPS / CONS_CLOCK_PER_SEC;
    _cons_cur_key   = (cons_key_t)getch();
    getmaxyx(stdscr, _cons_screen_height, _cons_screen_width);
}

/// 毎フレームの最後に行う処理. だいたい FPS 間隔になるように待つ.
void cons_updateEnd(void) {
    cons_clock_t now  = _cons_getClock();
    cons_clock_t next = (_cons_fps_count + 1) * CONS_CLOCK_PER_SEC / CONS_FPS;
    if (now < next) {
        cons_clock_t dif = next - now;
        cons_clock_sleep(dif);
        do {
            now = _cons_getClock();
        } while (now < next);
    }
    refresh();
}
