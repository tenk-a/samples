// カーソル移動できる Hello world! 表示. ESC か Q で終了.
#if defined(_WIN32)
 #include <windows.h>
 #include <curses.h>
#elif defined(__DOS__)
 #include <curses.h>
#else   // mac,linux,unix
 #include <ncurses.h>
 #include <unistd.h>
#endif
#include <time.h>

#define FPS     30

void fps_wait(unsigned long* pFpsCount);

int main(void) {
    // 初期化.
    enum { N = 12};                                     // Hello world! 文字数.
    int  x, y, w, h;
    unsigned long fps_count = 0;                        // FPS カウンタ.
    initscr();                                          // curses:(スクリーン)初期化.
    noecho();                                           // curses:キー入力で表示を行わない.
    cbreak();                                           // curses:入力バッファリングしない.
    keypad(stdscr, TRUE);                               // curses:カーソルキーを有効にする.
    nodelay(stdscr, TRUE);                              // curses:非ブロッキング入力.
    curs_set(0);                                        // curses:カーソルを表示しない.
    getmaxyx(stdscr, h, w);                             // curses:画面サイズ取得.
    x = (w - N) / 2;                                    // x初期位置:画面中央.
    y = (h - 1) / 2;                                    // y初期位置:画面中央.

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
        fps_wait(&fps_count);                           // FPS になるように待つ.
        refresh();                                      // curses:画面バッファを実画面に反映.
    }
    // 終了.
    endwin();                                           // curses 終了.
    return 0;
}

// clock_t の単位で sleep 指定.
static void clock_sleep(clock_t count) {
 #if defined(_WIN32)
    count = count * 1000 / CLOCKS_PER_SEC;
    if (count) Sleep(count);
 #elif defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 199309L) || defined(__APPLE__)
    struct timespec ts;
    ts.tv_sec  = count / CLOCKS_PER_SEC;
    count %= CLOCKS_PER_SEC;
    ts.tv_nsec = (long)(count * (1000000000LL / CLOCKS_PER_SEC));
    nanosleep(&ts, &ts);
 #elif !defined(__DOS__)
    count = count * 1000000LL / CLOCKS_PER_SEC;
    usleep(count);
 #endif
}

// だいたい FPS 間隔になるように待つ.
void fps_wait(unsigned long* pFpsCount) {
    clock_t now  = clock();
    clock_t next = (*pFpsCount + 1) * CLOCKS_PER_SEC / FPS;
    clock_t dif  = (next > now) ? next - now : 0;
    clock_sleep(dif);
    do {
        now = clock();
    } while (now < next);
    *pFpsCount = now * FPS / CLOCKS_PER_SEC;
}
