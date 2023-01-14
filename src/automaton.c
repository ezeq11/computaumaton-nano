#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.h"

#define COMPONENT_HEADER_WIDTH 7

static void component_header_draw(int y,int x,bool is_focus,char c1,char c2){
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
	chain_update(&(a->link_heads[a->focus]),in,a->link_relations,FSA_LINK_RELATIONS_COUNT);
	
	if(in == KEY_UP || in == KEY_DOWN){
		a->focus = (enum fsa_focus)(((int)FSA_FOCUS_COUNT + (int)a->focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FSA_FOCUS_COUNT);
	}
}

void fsa_draw(const struct fsa *a,int y,int x){
	for(uint i = 0;i < FSA_FOCUS_COUNT;++i){
		if(
			(a->link_heads[a->focus].read == LINK_IDEMPOTENT) ||
			(i == a->focus) ||
			(!chain_to_enqueue_has_super(&(a->link_heads[a->focus]),a->link_relations,FSA_LINK_RELATIONS_COUNT)) ||
			chain_contains_to_enqueue_super(
				&(a->link_heads[a->focus]),
				&(a->link_heads[i]),
				a->link_relations,
				FSA_LINK_RELATIONS_COUNT
			)
		){
			component_header_draw(
				y,x,
				a->focus == i,
				a->link_headers[i][0],a->link_headers[i][1]
			);
			
			y = chain_draw(&(a->link_heads[i]),y,x + COMPONENT_HEADER_WIDTH);
		}else{
			y = chain_nodraw(&(a->link_heads[i]),y);
		}
	}
}