#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.h"

struct fsa a = FSA_INIT(a);

int main(){
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);
	
	int in = 0;
	
	while(in != 'q'){
		clear();
		fsa_draw(2,2,&a);
		refresh();
		
		in = getch();
		fsa_update(&a,in);
	}
	
	clear();
	refresh();
	endwin();
	
	printf("fsa: %d bytes\n",sizeof(struct fsa));
	
	return EXIT_SUCCESS;
}