#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

// ------------------------------------------------------------ ||

int current_delay = -1;

// ------------------------------------------------------------ ||

fsa *fsa::current_callback_fsa = NULL;

void fsa::on_set_remove_callback(const set *s,symb val){
	if(s == &(current_callback_fsa->S)){
		current_callback_fsa->D.remove_containing(1,val);
		
	}else if (s == &(current_callback_fsa->Q)){
		current_callback_fsa->q0.remove_containing(0,val);
		
		current_callback_fsa->D.remove_containing(0,val);
		current_callback_fsa->D.remove_containing(2,val);
		
		current_callback_fsa->F.remove_containing(0,val);
	}
}

// -----------

void fsa::preupdate(){
	current_callback_fsa = this;
}

bool fsa::presimulate_check() const{
	return true;
}

void fsa::presimulate(){
	// Nothing
}

void fsa::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[3] = {tape_in.get_state(),tape_in.get_read(),SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

bool fsa::simulate_step_taken(){
	return tape_in.simulate(D.filter_results() > 0 ? D.filter_nav_select()[2] : tape_in.get_state());
}

// -----------

fsa::fsa():
	automaton<5>(&q0,&D,&S,NULL,&tape_in),
	S(' ','S',&on_set_remove_callback),Q(' ','Q',&on_set_remove_callback),q0('q','0'),D(' ','D'),F(' ','F',NULL)
{
	// Superset linking
	q0.set_superset(0,&Q);
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&Q);
	F.set_superset(0,&Q);
	
	tape_in.set_superset(&S);
	
	// Interface table population
	interfaces[0] = &S;
	interfaces[1] = &Q;
	interfaces[2] = &q0;
	interfaces[3] = &D;
	interfaces[4] = &F;
}