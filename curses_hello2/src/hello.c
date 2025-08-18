// カーソル移動できる Hello world! 表示. ESC か Q で終了.
#if defined(_WIN32)
 #include <windows.h>
 #include <curses.h>
 typedef unsigned __int64 myclock_t;
 #define MYCLOCK_PER_SEC  1000000LL
#elif defined(__DOS__)
 #include <curses.h>
 #include <sys/timeb.h>
 typedef unsigned long    myclock_t;
 #define MYCLOCK_PER_SEC  1000
#else   // linux,unix
 #include <time.h>
 #include <sys/time.h>
 #include <ncurses.h>
 typedef unsigned long long myclock_t;
 #define MYCLOCK_PER_SEC  1000000LL
#endif

#define FPS     60

void fps_wait(myclock_t* pFpsCount);

int main(void) {
    // 初期化.
    enum { N = 12};                                     // Hello world! 文字数.
    int  x, y, w, h;
    myclock_t fps_count = 0;                            // FPS カウンタ.
    initscr();                                          // curses:(スクリーン)初期化.
    noecho();                                           // curses:キー入力で表示を行わない.
    cbreak();                                           // curses:入力バッファリングしない.
    keypad(stdscr, TRUE);                               // curses:カーソルキーを有効にする.
    curs_set(0);                                        // curses:カーソルを表示しない.
    getmaxyx(stdscr, h, w);                             // curses:画面サイズ取得.
    x = (w - N) / 2;                                    // x初期位置:画面中央.
    y = (h - 1) / 2;                                    // y初期位置:画面中央.
    nodelay(stdscr, TRUE);                              // curses:非ブロッキング入力.

    for (;;) {
        int k = getch();                                // curses:1文字入力.待ちなし.
        if (k == 0x1b || k == 'q' || k == 'Q')          // ESCまたは Q キーで終了.
            break;
        x = x - (k == KEY_LEFT) + (k == KEY_RIGHT);     // 左右カーソルキーで増減.
        y = y - (k == KEY_UP  ) + (k == KEY_DOWN );     // 上下カーソルキーで増減.
        x = (x < 0) ? 0 : (x > w - N) ? (w - N) : x;    // x移動範囲チェック.
        y = (y < 0) ? 0 : (y > h - 1) ? (h - 1) : y;    // y移動範囲チェック.

        erase();                                        // curses:画面バッファ クリア.
        move(y, x);                                     // curses:表示位置設定.
        if (fps_count & 0x0c)                           // フレーム数をみて点滅させる.
            addstr("Hello world!");                     // curses:Hello world! 表示.
        refresh();                                      // curses:画面バッファを実画面に反映.
        fps_wait(&fps_count);                           // FPS になるように待つ.
    }
    // 終了.
    endwin();                                           // curses 終了.
    return 0;
}

// 現在の時間(マイクロ秒|ミリ秒)を取得.
myclock_t myclock_get(void) {
 #if defined(_WIN32)
    static unsigned __int64 per_sec = 0;
    unsigned __int64        count   = 0;
    if (!per_sec)
        QueryPerformanceFrequency((LARGE_INTEGER*)&per_sec);
    QueryPerformanceCounter((LARGE_INTEGER*)&count);
    return count * MYCLOCK_PER_SEC / per_sec;
 #elif defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (myclock_t)ts.tv_sec * 1000000ULL + (ts.tv_nsec / 1000ULL);
 #else
    struct timeb tmb = {0};
    ftime(&tmb);
    return tmb.time * 1000 + tmb.millitm;
 #endif
}

// myclock_t の単位で sleep.
void myclock_sleep(myclock_t count) {
 #if defined(_WIN32)
    count = count * 1000 / MYCLOCK_PER_SEC;
    if (count) Sleep(count);
 #elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L) || defined(__APPLE__)
    struct timespec ts;
    ts.tv_sec  = count / MYCLOCK_PER_SEC;
    count %= MYCLOCK_PER_SEC;
    ts.tv_nsec = (long)(count * (1000000000LL / MYCLOCK_PER_SEC));
    nanosleep(&ts, &ts);
 #elif !defined(__DOS__)
    count = count * 1000000LL / MYCLOCK_PER_SEC;
    if (count) usleep(count);
 #endif
}

// だいたい FPS 間隔になるように待つ.
void fps_wait(myclock_t* pFpsCount) {
    myclock_t now  = myclock_get();
    myclock_t next = (*pFpsCount + 1) * MYCLOCK_PER_SEC / FPS;
    if (now < next) {
        myclock_t dif = next - now;
        myclock_sleep(dif);
        do {
            now = myclock_get();
        } while (now < next);
    }
    *pFpsCount = now * FPS / MYCLOCK_PER_SEC;
}
