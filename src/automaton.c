#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.h"

void fsa_update(struct fsa *a,int in){
	bool is_switching = (in == KEY_UP || in == KEY_DOWN);
	
	switch(a->state){
	case AUT_STATE_IDLE:
		switch(a->focus){
		case FSA_FOCUS_S:
			set_update(&(a->S),in,is_switching);
			
			break;
		case FSA_FOCUS_Q:
			set_update(&(a->Q),in,is_switching);
			
			break;
		case FSA_FOCUS_Q0:
			element_update(&(a->q0),in,is_switching);
			
			break;
		case FSA_FOCUS_D:
			product_update(&(a->D0),in,is_switching);
			
			break;
		case FSA_FOCUS_F:
			set_update(&(a->F),in,is_switching);
			
			break;
		case FSA_FOCUS_COUNT:
			break;
		}
		
		if(in == KEY_UP || in == KEY_DOWN){
			a->focus = (enum fsa_focus)(((int)FSA_FOCUS_COUNT + (int)a->focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FSA_FOCUS_COUNT);
		}
		
		break;
	case AUT_STATE_STEPPING:
		
		break;
	}
}

void fsa_draw(int y,int x,const struct fsa *a){
	set_draw    (y + 0 ,x + 2,&(a->S));
	set_draw    (y + 3 ,x + 2,&(a->Q));
	element_draw(y + 5 ,x + 2,&(a->q0));
	product_draw(y + 7 ,x + 2,&(a->D0),8);
	set_draw    (y + 18,x + 2,&(a->F));
	
	switch(a->focus){
	case FSA_FOCUS_S:
		mvaddch(y + 0,x,'>');
		
		break;
	case FSA_FOCUS_Q:
		mvaddch(y + 3,x,'>');
		
		break;
	case FSA_FOCUS_Q0:
		mvaddch(y + 5,x,'>');
		
		break;
	case FSA_FOCUS_D:
		mvaddch(y + 7,x,'>');
		
		break;
	case FSA_FOCUS_F:
		mvaddch(y + 18,x,'>');
		
		break;
	case FSA_FOCUS_COUNT:
		break;
	}
}