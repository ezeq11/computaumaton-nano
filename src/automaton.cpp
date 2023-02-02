#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

fsa::fsa()
:state(AUT_STATE_IDLE),current_focus(FOCUS_S),interfaces{&S,&Q,&q0,&D,&F},S(' ','S',NULL),Q(' ','Q',NULL),q0('q','0'),D(' ','D'),F(' ','F',NULL){
	
}

void fsa::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[3] = {tape_in.get_state(),tape_in.get_read(),SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

bool fsa::simulate_step_take(){
	return tape_in.simulate(D.filter_results() > 0 ? D.filter_nav_select()[2] : tape_in.get_state());
}

void fsa::update(int in){
	switch(state){
	case AUT_STATE_IDLE:
		if(!(interfaces[current_focus]->edit((char)in))){
			return;
		}
		
		switch(in){
		case KEY_UP:
		case KEY_DOWN:
			current_focus = (focus)(((int)FOCUS_COUNT + (int)current_focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FOCUS_COUNT);
			
			break;
		case ':':
			if(!q0.is_set()){
				break;
			}
			
			tape_in.init_edit();
			state = AUT_STATE_TAPE_INPUT;
			
			break;
		}
		
		break;
	case AUT_STATE_TAPE_INPUT:
		switch(in){
		case '`':
			state = AUT_STATE_IDLE;
			
			break;
		case ' ':
			tape_in.init_simulate(q0.get());
			simulate_step_filter();
			
			if(D.filter_results() > 1){
				state = AUT_STATE_STEPPING_SELECTION;
			}else{
				state = AUT_STATE_STEPPING;
			}
			
			break;
		case '\t':
		case '\n':
			tape_in.init_simulate(q0.get());
			simulate_step_filter();
			
			if(D.filter_results() > 1){
				timeout(-1);
				state = AUT_STATE_SIMULATING_SELECTION;
			}else{
				timeout(200);
				state = AUT_STATE_SIMULATING;
			}
			
			break;
		default:
			tape_in.edit(in);
			
			break;
		}
		
		break;
	case AUT_STATE_STEPPING:
	case AUT_STATE_SIMULATING:
		if(in == '`'){
			if(state == AUT_STATE_SIMULATING){
				timeout(-1);
			}
			
			D.filter_clear();
			state = AUT_STATE_TAPE_INPUT;
			
		}else if(state == AUT_STATE_SIMULATING || (state == AUT_STATE_STEPPING && in == ' ')){
			if(simulate_step_take()){
				if(state == AUT_STATE_SIMULATING){
					timeout(-1);
				}
				
				D.filter_clear();
				state = AUT_STATE_HALTED;
				
			}else{
				simulate_step_filter();
				
				if(D.filter_results() > 1){
					if(state == AUT_STATE_SIMULATING){
						timeout(-1);
					}
					
					state = (state == AUT_STATE_STEPPING) ? AUT_STATE_STEPPING_SELECTION : AUT_STATE_SIMULATING_SELECTION;
				}
			}
		}
		
		break;
	case AUT_STATE_STEPPING_SELECTION:
	case AUT_STATE_SIMULATING_SELECTION:
		switch(in){
		case KEY_UP:
			D.filter_nav_prev();
			
			break;
		case KEY_DOWN:
			D.filter_nav_next();
			
			break;
		case ' ':
			if(state != AUT_STATE_STEPPING_SELECTION){
				break;
			}
			
			if(simulate_step_take()){
				D.filter_clear();
				state = AUT_STATE_HALTED;
			}else{
				simulate_step_filter();
				state = AUT_STATE_STEPPING;
			}
			
			break;
		case '\t':
		case '\n':
			if(state != AUT_STATE_SIMULATING_SELECTION){
				break;
			}
			
			if(simulate_step_take()){
				D.filter_clear();
				state = AUT_STATE_HALTED;
			}else{
				simulate_step_filter();
				timeout(200);
				
				state = AUT_STATE_SIMULATING;
			}
			
			break;
		}
		
		break;
	case AUT_STATE_HALTED:
		switch(in){
		case '`':
		case ' ':
		case '\t':
		case '\n':
			state = AUT_STATE_TAPE_INPUT;
		}
		
		break;
	}
}

int fsa::draw(int y,int x) const{
	// Components
	for(uint i = 0;i < FOCUS_COUNT;++i){
		y = interfaces[i]->draw(y,x,i == (uint)current_focus);
	}
	
	// Tape input
	if(state != AUT_STATE_IDLE){
		y = tape_in.draw(y,x,state != AUT_STATE_TAPE_INPUT);
	}else{
		y = tape_in.nodraw(y);
	}
	
	// Done
	return y;
}