#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

fsa a;

int main(){
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);
	
	int in = 0;
	
	while(in != 0x1b){
		clear();
		a.draw(2,2);
		refresh();
		
		in = getch();
		a.update(in);
	}
	
	clear();
	refresh();
	endwin();
	
	printf("fsa: %d bytes\n",sizeof(fsa));
	
	return EXIT_SUCCESS;
}