#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "set.h"
#include "automaton.h"
#include "draw.h"

struct fsa a = FSA_INIT(a);

int main(){
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	
	set_add(&(a.S),charset('a'));
	set_add(&(a.Q),charset('b'));
	a.D0.q_element = 4;
	
	int in = 0;
	
	while(in != 'q'){
		clear();
		draw_fsa(3,5,&a);
		refresh();
		
		in = getch();
		fsa_update(&a,in);
	}
	
	endwin();
	
	return EXIT_SUCCESS;
}