#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

fsa::fsa():
	state(AUT_STATE_IDLE),current_focus(FOCUS_S),
	
	interfaces{&S,&Q,&q0,&D,&F},
	S(' ','S',NULL),Q(' ','Q',NULL),q0('q','0'),D(' ','D'),F(' ','F',NULL)
{
	
}

void fsa::simulating_timeout(int delay) const{
	static int cur_delay = -1;
	
	if(state != AUT_STATE_SIMULATING){
		return;
	}
	
	if(delay == cur_delay){
		return;
	}
	
	timeout(delay);
	cur_delay = delay;
}

void fsa::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[3] = {tape_in.get_state(),tape_in.get_read(),SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

bool fsa::simulate_step_taken(){
	return tape_in.simulate(D.filter_results() > 0 ? D.filter_nav_select()[2] : tape_in.get_state());
}

void fsa::simulation_filter(){
	simulate_step_filter();
	
	if(simulation_selecting()){
		simulating_timeout(-1);
	}else{
		simulating_timeout(200);
	}
}

bool fsa::simulation_selecting() const{
	return D.filter_results() > 1;
}

void fsa::simulation_end(automaton_state new_state){
	simulating_timeout(-1);
	D.filter_clear();
	
	state = new_state;
}

void fsa::update(int in){
	switch(state){
	case AUT_STATE_IDLE:
		interfaces[current_focus]->edit((char)in);
		
		if(!(interfaces[current_focus]->edit_interruptible())){
			break;
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
		case '\t':
			if(!(tape_in.can_simulate())){
				break;
			}
			
			state = (in == ' ') ? AUT_STATE_STEPPING : AUT_STATE_SIMULATING;
			
			tape_in.init_simulate(q0.get());
			simulation_filter();
			
			break;
		default:
			tape_in.edit(in);
			
			break;
		}
		
		break;
	case AUT_STATE_STEPPING:
	case AUT_STATE_SIMULATING:
		if(in == '`'){
			// Escape
			simulation_end(AUT_STATE_TAPE_INPUT);
			
		}else if(
			(state == AUT_STATE_STEPPING && in == ' ') ||
			(state == AUT_STATE_SIMULATING && (!simulation_selecting() || in == '\t'))
		){
			// Select current transition
			if(simulate_step_taken()){
				simulation_end(AUT_STATE_HALTED);
				
			}else{
				simulation_filter();
				
			}
		}else if(simulation_selecting()){
			// Navigate currently applicable transitions
			switch(in){
			case KEY_UP:
				D.filter_nav_prev();
				
				break;
			case KEY_DOWN:
				D.filter_nav_next();
				
				break;
			}
		}
		
		break;
	case AUT_STATE_HALTED:
		if(in == '\n'){
			state = AUT_STATE_TAPE_INPUT;
		}
		
		break;
	}
}

int fsa::draw(int y,int x) const{
	// Components
	for(uint i = 0;i < FOCUS_COUNT;++i){
		y = interfaces[i]->draw(y,x,(state == AUT_STATE_IDLE) && (i == (uint)current_focus),simulation_selecting());
	}
	
	// Tape input
	if(state != AUT_STATE_IDLE){
		y = tape_in.draw(y,x,!simulation_selecting(),state != AUT_STATE_TAPE_INPUT);
	}else{
		y = tape_in.nodraw(y);
	}
	
	// Available commands
	move(y,x);
	printw("|--- esc --- ");
	
	switch(state){
	case AUT_STATE_IDLE:
		if(interfaces[(uint)current_focus]->edit_interruptible()){
			printw(q0.is_set() ? ": " : "# ");
		}
		
		interfaces[(uint)current_focus]->print_available_commands();
		
		if(interfaces[(uint)current_focus]->edit_interruptible()){
			printw("--- up down ---| idle");
		}
		
		break;
	case AUT_STATE_TAPE_INPUT:
		printw(tape_in.can_simulate() ? "` tab space --- " : "` ### ##### --- ");
		tape_in.print_available_commands();
		printw("---| tape input");
		
		break;
	case AUT_STATE_STEPPING:
	case AUT_STATE_SIMULATING:
		printw("` ");
		
		if(state == AUT_STATE_STEPPING){
			printw("space --- ");
		}else if(simulation_selecting()){
			printw("tab --- ");
		}else{
			printw("### --- ");
		}
		
		printw(simulation_selecting() ? "up down " : "## #### ");
		printw(state == AUT_STATE_STEPPING ? "---| stepping" : "---| simulating");
		
		break;
	case AUT_STATE_HALTED:
		printw("enter ---| halted");
		
		break;
	}
	
	y += 2;
	
	// Done
	return y;
}