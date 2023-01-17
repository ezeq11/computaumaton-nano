#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "symbol.h"
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
	switch(a->state){
	case AUT_STATE_IDLE:
		if(!chain_update(&(a->link_heads[a->focus]),in,a->link_relations,FSA_LINK_RELATIONS_COUNT)){
			break;
		}
		
		switch(in){
		case KEY_UP:
		case KEY_DOWN:
			a->focus = (enum fsa_focus)(((int)FSA_FOCUS_COUNT + (int)a->focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FSA_FOCUS_COUNT);
			
			break;
		case ':':
			symb_list_clear(&(a->in));
			a->state = AUT_STATE_CONFIGURING;
			
			break;
		}
		
		break;
	case AUT_STATE_CONFIGURING:
		switch(in){
		case '\b':
		case 0x7f:
			symb_list_pop(&(a->in));
			
			break;
		case '\t':
		case '\n':			
			a->in_pos = 0;
			timeout(200);
			
			a->state = AUT_STATE_SIMULATING;
			
			break;
		case ' ':
			a->in_pos = 0;
			a->state = AUT_STATE_STEPPING;
			
			break;
		case '`':
			a->state = AUT_STATE_IDLE;
			
			break;
		default:
			if(is_symbol((char)in)){
				symb new_val = symbol((char)in);
				
				if(set_contains(&(a->S),new_val)){
					symb_list_push(&(a->in),new_val);
				}
			}
			
			break;
		}
		
		break;
	case AUT_STATE_STEPPING:
		switch(in){
		case ' ':
			if(a->in_pos >= symb_list_len(&(a->in))){
				a->state = AUT_STATE_CONFIGURING;
			}
			
			++(a->in_pos);
			
			break;
		case '`':
			a->state = AUT_STATE_CONFIGURING;
			
			break;
		}
		
		break;
	case AUT_STATE_SIMULATING:
		if(in == '`' || a->in_pos >= symb_list_len(&(a->in))){
			timeout(-1);
			a->state = AUT_STATE_CONFIGURING;
			
			break;
		}
		
		++(a->in_pos);
		
		break;
	}
}

static void symb_list_draw(uint i,symb val){
	addch(ascii(val));
}

void fsa_draw(const struct fsa *a,int y,int x){
	// Mathematical components
	bool draw_component[FSA_FOCUS_COUNT];
	bool cursor_at[FSA_FOCUS_COUNT];
	
	for(uint i = 0;i < FSA_FOCUS_COUNT;++i){
		switch(a->state){
		case AUT_STATE_IDLE:
			draw_component[i] =
				(a->link_heads[a->focus].read == LINK_IDEMPOTENT) ||
				(i == a->focus) ||
				(!chain_to_enqueue_has_super(&(a->link_heads[a->focus]),a->link_relations,FSA_LINK_RELATIONS_COUNT)) ||
				chain_contains_to_enqueue_super(
					&(a->link_heads[a->focus]),
					&(a->link_heads[i]),
					a->link_relations,
					FSA_LINK_RELATIONS_COUNT
				);
			
			cursor_at[i] = (a->focus == i);
			
			break;
		case AUT_STATE_CONFIGURING:
			draw_component[i] = (i == FSA_FOCUS_S);
			cursor_at[i] = 0;
			
			break;
		case AUT_STATE_STEPPING:
		case AUT_STATE_SIMULATING:
			draw_component[i] = 1;
			cursor_at[i] = 0;
			
			break;
		}
	}
	
	for(uint i = 0;i < FSA_FOCUS_COUNT;++i){
		if(draw_component[i]){
			component_header_draw(y,x,cursor_at[i],a->link_headers[i][0],a->link_headers[i][1]);
			y = chain_draw(&(a->link_heads[i]),y,x + COMPONENT_HEADER_WIDTH);
		}else{
			y = chain_nodraw(&(a->link_heads[i]),y);
		}
	}
	
	// Configuration state-cursor
	y += 2;
	
	switch(a->state){
	case AUT_STATE_IDLE:
	case AUT_STATE_CONFIGURING:
		y += 2;
		
		break;
	case AUT_STATE_STEPPING:
	case AUT_STATE_SIMULATING:
		move(y,x + COMPONENT_HEADER_WIDTH + a->in_pos);
		addch(ascii(a->q));
		++y;
		
		move(y,x + COMPONENT_HEADER_WIDTH + a->in_pos);
		addch('v');
		++y;
		
		break;
	}
	
	// Configuration input
	switch(a->state){
	case AUT_STATE_IDLE:
		break;
	case AUT_STATE_CONFIGURING:
	case AUT_STATE_STEPPING:
	case AUT_STATE_SIMULATING:
		move(y,x + COMPONENT_HEADER_WIDTH - 2);
		addch(':');
		
		move(y,x + COMPONENT_HEADER_WIDTH);
		symb_list_forall(&(a->in),&symb_list_draw);
		
		break;
	}
	
	y += 2;
}