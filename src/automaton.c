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

const struct set *current_superset;

#define AUT_MAYBE_DRAW_SET(a,y,x,c1,c2,struct_name,focus_const) {\
		bool visible = current_superset == NULL || current_superset == &(a->struct_name) || a->focus == focus_const;\
		\
		if(visible){\
			component_header_draw(y,x,c1,c2,a->focus == focus_const);\
			y = set_draw(y,x + COMPONENT_HEADER_WIDTH,&(a->struct_name));\
		}else{\
			y = set_nodraw(y);\
		}\
	}

#define AUT_MAYBE_DRAW_ELEMENT(a,y,x,c1,c2,struct_name,focus_const) {\
		bool visible = current_superset == NULL || a->focus == focus_const;\
		\
		if(visible){\
			component_header_draw(y,x,c1,c2,a->focus == focus_const);\
			y = element_draw(y,x + COMPONENT_HEADER_WIDTH,&(a->struct_name));\
		}else{\
			y = element_nodraw(y);\
		}\
	}

#define AUT_MAYBE_DRAW_PRODUCT(a,y,x,c1,c2,struct_name,focus_const) {\
		bool visible = current_superset == NULL || a->focus == focus_const;\
		\
		if(visible){\
			component_header_draw(y,x,c1,c2,a->focus == focus_const);\
			y = product_draw(y,x + COMPONENT_HEADER_WIDTH,&(a->struct_name),8);\
		}else{\
			y = product_nodraw(y,&(a->struct_name),8);\
		}\
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

void fsa_update(struct fsa *a,int in){
	chain_update(a->link_heads + a->focus,in,a->link_relations,5);
	
	if(in == KEY_UP || in == KEY_DOWN){
		a->focus = (enum fsa_focus)(((int)FSA_FOCUS_COUNT + (int)a->focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FSA_FOCUS_COUNT);
	}
}

void fsa_draw(int y,int x,const struct fsa *a){
	current_superset = NULL;
	
	switch(a->focus){
		case FSA_FOCUS_S:
				current_superset = queue_read_superset(&(a->S.read));
				break;
		case FSA_FOCUS_Q:
				current_superset = queue_read_superset(&(a->Q.read));
				break;
		case FSA_FOCUS_Q0:
				current_superset = queue_read_superset(&(a->q0.read));
				break;
		case FSA_FOCUS_D:
				current_superset = queue_read_superset(&(a->D0.read));
				break;
		case FSA_FOCUS_F:
				current_superset = queue_read_superset(&(a->F.read));
				break;
		case FSA_FOCUS_COUNT:
				break;
	}
	
	AUT_MAYBE_DRAW_SET    (a,y,x,' ','S',S ,FSA_FOCUS_S )
	AUT_MAYBE_DRAW_SET    (a,y,x,' ','Q',Q ,FSA_FOCUS_Q )
	AUT_MAYBE_DRAW_ELEMENT(a,y,x,'q','0',q0,FSA_FOCUS_Q0)
	AUT_MAYBE_DRAW_PRODUCT(a,y,x,' ','D',D0,FSA_FOCUS_D )
	AUT_MAYBE_DRAW_SET    (a,y,x,' ','F',F ,FSA_FOCUS_F )
}

void fsa_draw_help(int x,const struct fsa *a){
	int scr_h,scr_w;
	getmaxyx(stdscr,scr_h,scr_w);
	
	int y = scr_h - 2;
	
	move(y,x);
	addstr("up/down : navigate");
	--y;
	
	switch(a->focus){
		case FSA_FOCUS_S:
				set_draw_help(y,x,&(a->S));
				break;
		case FSA_FOCUS_Q:
				set_draw_help(y,x,&(a->Q));
				break;
		case FSA_FOCUS_Q0:
				element_draw_help(y,x,&(a->q0));
				break;
		case FSA_FOCUS_D:
				product_draw_help(y,x,&(a->D0));
				break;
		case FSA_FOCUS_F:
				set_draw_help(y,x,&(a->F));
				break;
		case FSA_FOCUS_COUNT:
				break;
	}
}