#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "symbol.h"
#include "automaton.h"

static void set_update(enum automaton_edit *edit,struct set *s,int in){
	if(in == KEY_UP || in == KEY_DOWN){
		set_q_clear(s);
		
	}else{
		if(*edit == AUT_EDIT_IDEMPOTENT){
			switch(in){
			case 'U':
			case 'u':
				set_q_clear(s);
				*edit = AUT_EDIT_UNION;
				
				break;
			case '\\':
				set_q_clear(s);
				*edit = AUT_EDIT_DIFFERENCE;
				
				break;
			default:
				break;
			}
			
		}else{
			switch(in){
			case '\b':
				set_q_dequeue(s);
				
				break;
			case '\t':
			case '\n':
				switch(*edit){
					case AUT_EDIT_UNION:
						if(set_q_add(s)){
							if(in == '\t'){
								set_q_clear(s);
							}else{
								*edit = AUT_EDIT_IDEMPOTENT;
							}
						}
						
						break;
					case AUT_EDIT_DIFFERENCE:
						if(set_q_remove(s)){
							if(in == '\t'){
								set_q_clear(s);
							}else{
								*edit = AUT_EDIT_IDEMPOTENT;
							}
						}
						
						break;
					case AUT_EDIT_IDEMPOTENT:
					default:
						break;
				}
				
				break;
			case '`':
				*edit = AUT_EDIT_IDEMPOTENT;
				
				break;
			default:
				if(is_symbol(in)){
					set_q_enqueue(s,symbol((char)in));
				}
				
				break;
			}
		}
	}
}

static void element_update(enum automaton_edit *edit,struct element *e,int in){
	if(in == KEY_UP || in == KEY_DOWN){
		element_q_clear(e);
		
	}else{
		if(*edit == AUT_EDIT_IDEMPOTENT){
			switch(in){
			case '=':
				element_q_clear(e);
				*edit = AUT_EDIT_UNION;
				
				break;
			default:
				break;
			}
			
		}else{
			switch(in){
			case '\b':
				element_q_dequeue(e);
				
				break;
			case '\t':
			case '\n':
				switch(*edit){
					case AUT_EDIT_UNION:
						if(element_q_add(e)){
							*edit = AUT_EDIT_IDEMPOTENT;
						}
						
						break;
					case AUT_EDIT_DIFFERENCE:
					case AUT_EDIT_IDEMPOTENT:
					default:
						break;
				}
				
				break;
			case '`':
				*edit = AUT_EDIT_IDEMPOTENT;
				
				break;
			default:
				if(is_symbol(in)){
					element_q_enqueue(e,symbol((char)in));
				}
				
				break;
			}
		}
	}
}

void fsa_update(struct fsa *a,int in){
	switch(a->state){
	case AUT_STATE_IDLE:
		switch(a->focus){
		case FSA_FOCUS_S:
			set_update(&(a->edit),&(a->S),in);
			
			break;
		case FSA_FOCUS_Q:
			set_update(&(a->edit),&(a->Q),in);
			
			break;
		case FSA_FOCUS_Q0:
			element_update(&(a->edit),&(a->q0),in);
			
			break;
		case FSA_FOCUS_D:
			product_update(&(a->D0),in,in == KEY_UP || in == KEY_DOWN);
			
			break;
		case FSA_FOCUS_F:
			set_update(&(a->edit),&(a->F),in);
			
			break;
		case FSA_FOCUS_COUNT:
		default:
			break;
		}
		
		if(in == KEY_UP || in == KEY_DOWN){
			a->edit = AUT_EDIT_IDEMPOTENT;
			a->focus = (enum fsa_focus)(((int)FSA_FOCUS_COUNT + (int)a->focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FSA_FOCUS_COUNT);
		}
		
		break;
	case AUT_STATE_STEPPING:
		
		break;
	default:
		break;
	}
}