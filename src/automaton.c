#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.h"

#define COMPONENT_HEADER_WIDTH 7

static void component_header_draw(int y,int x,char c1,char c2,bool is_focus){
	move(y,x);
	
	if(is_focus){
		addch('>');
	}else{
		addch(' ');
	}
	
	addch(' ');
	
	addch(c1);
	addch(c2);
	
	addch(' ');
	addch('=');
	addch(' ');
}

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
	int dy = y;
	
	component_header_draw(dy,x,' ','S',a->focus == FSA_FOCUS_S);
	dy = set_draw(dy,x + COMPONENT_HEADER_WIDTH,&(a->S));
	
	component_header_draw(dy,x,' ','Q',a->focus == FSA_FOCUS_Q);
	dy = set_draw(dy,x + COMPONENT_HEADER_WIDTH,&(a->Q));
	
	component_header_draw(dy,x,'q','0',a->focus == FSA_FOCUS_Q0);
	dy = element_draw(dy,x + COMPONENT_HEADER_WIDTH,&(a->q0));
	
	component_header_draw(dy,x,' ','D',a->focus == FSA_FOCUS_D);
	dy = product_draw(dy,x + COMPONENT_HEADER_WIDTH,&(a->D0),8);
	
	component_header_draw(dy,x,' ','F',a->focus == FSA_FOCUS_F);
	dy = set_draw(dy,x + COMPONENT_HEADER_WIDTH,&(a->F));
}