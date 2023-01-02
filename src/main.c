#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "charset.h"
#include "set.h"
#include "draw.h"

struct set s = SET_INIT(NULL,NULL,NULL);

int main(){
	set_add(&s,charset('a'));
	set_add(&s,charset('e'));
	set_add(&s,charset('i'));
	set_add(&s,charset('o'));
	set_add(&s,charset('u'));
	
	initscr();
	
	draw_set(3,5,&s);
	
	refresh();
	getch();
	endwin();
	
	return EXIT_SUCCESS;
}