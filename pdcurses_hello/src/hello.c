#if defined(USE_PDCURSES)
#include <curses.h>
#define CLEAR()		clear()
#else
#include <sys/time.h>
#include <ncurses.h>
#define CLEAR()		erase()
#endif

int main(void) {
	int w, h, x, y, x0, y0, col = 0, count = 0;
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(50);
	if (has_colors()) {
	    start_color();
	    init_pair( 1,COLOR_CYAN,  COLOR_BLACK);
	    init_pair( 2,COLOR_WHITE, COLOR_BLACK);
	    col = 1;
    }
    getmaxyx(stdscr, h, w);
	x0 = (w - 12) / 2;
	y0 = (h -  1) / 2;
	do {
		CLEAR();
		x = -8 + ((count & 0x10) ? 15 - (count & 15) : (count & 15));
		y = -4 + ((count & 0x8) ? 7 - (count & 7) : (count & 7));
		move(y0+y, x0+x);
		if (col) {
			col = (count & 0x0c) ? 1 : 2;
			attron(COLOR_PAIR(col));
		}
		addstr("hello world!");
		if (col)
			attroff(COLOR_PAIR(col));
		++count;
	} while (getch() == ERR);

	endwin();
	return 0;
}
