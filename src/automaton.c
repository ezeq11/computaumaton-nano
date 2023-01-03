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

static void product_update(enum automaton_edit *edit,struct product *p,int in){
	if(in == KEY_UP || in == KEY_DOWN){
		product_q_clear(p);
		
	}else{
		if(*edit == AUT_EDIT_IDEMPOTENT){
			switch(in){
			case 'U':
			case 'u':
				product_q_clear(p);
				*edit = AUT_EDIT_UNION;
				
				break;
			case '\\':
				product_q_clear(p);
				*edit = AUT_EDIT_DIFFERENCE;
				
				break;
			default:
				break;
			}
			
		}else{
			switch(in){
			case '\b':
				product_q_dequeue(p);
				
				break;
			case '\t':
			case '\n':
				switch(*edit){
					case AUT_EDIT_UNION:
						if(product_q_add(p)){
							if(in == '\t'){
								product_q_clear(p);
							}else{
								*edit = AUT_EDIT_IDEMPOTENT;
							}
						}
						
						break;
					case AUT_EDIT_DIFFERENCE:
						if(product_q_remove(p)){
							if(in == '\t'){
								product_q_clear(p);
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
					product_q_enqueue(p,symbol((char)in));
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
			
			break;
		case FSA_FOCUS_D:
			product_update(&(a->edit),&(a->D0),in);
			
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