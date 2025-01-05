// カーソル移動できる Hello world! 表示. ESC か Q で終了.
#if defined(USE_PDCURSES)
#include <curses.h>
#define CLEAR()     clear()                             // pdcurses での描画バッファ消去.
#else
#include <ncurses.h>
#define CLEAR()     erase()                             // ncurses での描画バッファ消去.
#endif

int main(void) {
    enum { N = 12};                                     // Hello world! 文字数.
    int  c, x, y, w, h, col = 0, count = 0;
    initscr();                                          // curses (スクリーン)初期化.
    noecho();                                           // キー入力で表示を行わない.
    cbreak();                                           // 入力バッファリングしない.
    keypad(stdscr, TRUE);                               // カーソルキーを有効にする.
    curs_set(0);                                        // カーソルを表示しない.
    timeout(50);                                        // 50ミリ秒でgetchをタイムアウトさせる指定.
    if (has_colors()) {                                 // 色有りのスクリーンなら色設定.
        start_color();
        init_pair( 1, COLOR_WHITE, COLOR_BLACK);        // 色ペア1を白に.
        init_pair( 2, COLOR_CYAN,  COLOR_BLACK);        // 色ペア2を水色に.
        col = 1;
    }
    getmaxyx(stdscr, h, w);                             // 画面サイズ取得.
    x = (w - N) / 2;                                    // x初期位置:画面中央.
    y = (h - 1) / 2;                                    // y初期位置:画面中央.
    for (;;) {
        c = getch();                                    // 1文字入力. 50ミリ秒でタイムアウト.
        if (c == 0x1b || c == 'q' || c == 'Q')          // ESCまたは Q キーで終了.
            break;
        x = x - (c == KEY_LEFT) + (c == KEY_RIGHT);     // 左右カーソルキーで増減.
        y = y - (c == KEY_UP  ) + (c == KEY_DOWN);      // 上下カーソルキーで増減.
        x = (x < 0) ? 0 : (x > w - N) ? (w - N) : x;    // x移動範囲チェック.
        y = (y < 0) ? 0 : (y > h - 1) ? (h - 1) : y;    // y移動範囲チェック.

        CLEAR();                                        // 画面バッファクリア.
        move(y, x);                                     // 表示位置設定.
        if (col) {
            col = (count & 0x0c) ? 1 : 2;               // フレーム数依存で色変更.
            attron(COLOR_PAIR(col));                    // 色設定.
        }
        addstr("Hello world!");                         // hello world!表示.
        ++count;                                        // フレームカウンタ更新.
        refresh();                                      // 画面バッファを実画面に反映.
    }
    endwin();                                           // curses 終了.
    return 0;
}
