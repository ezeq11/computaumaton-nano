#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "unsigned.h"
#include "symbol.h"

#include "draw.h"

uint set_i;
uint set_size;

static void draw_set_member(uint i){
	addch(ascii(i));
	++set_i;
	
	if(set_i < set_size){
		addch(',');
		addch(' ');
	}
}

void draw_set(int y,int x,struct set *s,enum automaton_edit edit){
	set_i = 0;
	set_size = bit_array_size(s->members,SYMBOL_COUNT);
	
	move(y,x);
	
	addch('{');
	addch(' ');
	bit_array_forall(s->members,SYMBOL_COUNT,&draw_set_member);
	addch(' ');
	addch('}');
	
	switch(edit){
	case AUT_EDIT_UNION:
		addch(' ');
		addch('U');
		addch(' ');
		
		addch('{');
		addch(' ');
		addch(ascii(set_q_get(s)));
		addch(' ');
		addch('}');
		
		break;
	case AUT_EDIT_DIFFERENCE:
		addch(' ');
		addch('\\');
		addch(' ');
		
		addch('{');
		addch(' ');
		addch(ascii(set_q_get(s)));
		addch(' ');
		addch('}');
		
		break;
	case AUT_EDIT_IDEMPOTENT:
	default:
		break;
	}
}

void draw_element(int y,int x,struct element *e,enum automaton_edit edit){
	move(y,x);
	
	if(edit == AUT_EDIT_UNION){
		addch(ascii(element_q_get(e)));
	}else{
		addch(ascii(element_get(e)));
	}
}

void draw_fsa(int y,int x,struct fsa *a){
	draw_set    (y + 0 ,x + 2,&(a->S) ,a->focus == FSA_FOCUS_S  ? a->edit : AUT_EDIT_IDEMPOTENT);
	draw_set    (y + 3 ,x + 2,&(a->Q) ,a->focus == FSA_FOCUS_Q  ? a->edit : AUT_EDIT_IDEMPOTENT);
	draw_element(y + 5 ,x + 2,&(a->q0),a->focus == FSA_FOCUS_Q0 ? a->edit : AUT_EDIT_IDEMPOTENT);
	product_draw(y + 7 ,x + 2,&(a->D0),8);
	draw_set    (y + 18,x + 2,&(a->F) ,a->focus == FSA_FOCUS_F  ? a->edit : AUT_EDIT_IDEMPOTENT);
	
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
	default:
		break;
	}
}