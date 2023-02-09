#include <cstdlib>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "unsigned.hpp"
#include "automata.hpp"

// Global automata
fsa finite_state_automaton;
pda pushdown_automaton;
tm turing_machine;

automaton * const automata[3] = {&finite_state_automaton,&pushdown_automaton,&turing_machine};
uint current_automaton = 0;

bool illustrate_supersets = true;

// Program
int main(){
	tm::init();
	
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr,TRUE);
	
	int in = 0;
	
	while(in != 0x1b){
		// Draw
		clear();
		
		move(1,2);
		switch(current_automaton){
		case 0:
			printw("M = (S,Q,D,q0,F) ----------- | finite-state automaton");
			break;
		case 1:
			printw("M = (S,Q,G,D,q0,g0,F) ------ | pushdown automaton");
			break;
		case 2:
			printw("M = (S,Q,D,b,q0,F) --------- | turing machine");
			break;
		default:
			break;
		}
		
		printw(automata[current_automaton]->is_interruptible() ? " [left][right]" : " [    ][     ]");
		automata[current_automaton]->draw(3,2,illustrate_supersets);
		
		refresh();
		
		// Update
		in = getch();
		
		if(automata[current_automaton]->is_interruptible() && (in == KEY_LEFT || in == KEY_RIGHT)){
			current_automaton = (3 + current_automaton - (in == KEY_LEFT ? 1 : 0) + (in == KEY_RIGHT ? 1 : 0)) % 3;
		}else if(in == '?'){
			illustrate_supersets = !illustrate_supersets;
		}else{
			automata[current_automaton]->update(in);
		}
	}
	
	clear();
	refresh();
	endwin();
	
	printf("fsa: %d bytes\n",sizeof(fsa));
	printf("pda: %d bytes\n",sizeof(pda));
	printf("tm: %d bytes\n",sizeof(tm));
	
	return EXIT_SUCCESS;
}