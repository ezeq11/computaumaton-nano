#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

fsa::fsa()
:state(AUT_STATE_IDLE),current_focus(FOCUS_S),interfaces{&S,&Q,&q0,&D,&F},S(' ','S',NULL),Q(' ','Q',NULL),q0('q','0'),D(' ','D'),F(' ','F',NULL){
	
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
			if(q0.size() == 0){
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
			tape_in.init_simulate(q0.vertical_get(0,0));
			state = AUT_STATE_STEPPING;
			
			break;
		case '\t':
		case '\n':
			tape_in.init_simulate(q0.vertical_get(0,0));
			timeout(200);
			
			state = AUT_STATE_SIMULATING;
			
			break;
		default:
			tape_in.edit(in);
			
			break;
		}
		
		break;
	case AUT_STATE_STEPPING:
		switch(in){
		case '`':
			state = AUT_STATE_TAPE_INPUT;
			
			break;
		case ' ':
			if(tape_in.simulate(symbol('a'))){
				state = AUT_STATE_TAPE_INPUT;
			}
			
			break;
		}
		
		break;
	case AUT_STATE_SIMULATING:
		switch(in){
		case '`':
			timeout(-1);
			state = AUT_STATE_TAPE_INPUT;
			
			break;
		default:
			if(tape_in.simulate(symbol('a'))){
				timeout(-1);
				state = AUT_STATE_TAPE_INPUT;
			}
			
			break;
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