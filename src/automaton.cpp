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

int fsa::postdraw(int y,int x) const{
	return y;
}

bool fsa::halt_condition() const{
	return false;
}

void fsa::postsimulate(){
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

// ------------------------------------------------------------ ||

pda *pda::current_callback_pda = NULL;

void pda::on_set_remove_callback(const set *s,symb val){
	if(s == &(current_callback_pda->S)){
		current_callback_pda->D.remove_containing(1,val);
		
	}else if (s == &(current_callback_pda->Q)){
		current_callback_pda->D.remove_containing(0,val);
		current_callback_pda->D.remove_containing(3,val);
		
		current_callback_pda->q0.remove_containing(0,val);
		current_callback_pda->F.remove_containing(0,val);
		
	}else if(s == &(current_callback_pda->G)){
		current_callback_pda->D.remove_containing(2,val);
		
		for(uint j = 4;j < 12;++j){
			current_callback_pda->D.remove_containing(j,val);
		}
		
		current_callback_pda->g0.remove_containing(0,val);
	}
}

// -----------

void pda::preupdate(){
	current_callback_pda = this;
}

bool pda::presimulate_check() const{
	return g0.is_set();
}

void pda::presimulate(){
	stack_contents.clear();
	stack_contents.push(g0.get());
}

int pda::postdraw(int y,int x) const{
	return stack_contents.draw(y,x);
}

bool pda::halt_condition() const{
	return false;
}

void pda::postsimulate(){
	stack_contents.clear();
}

void pda::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[12] = {
		tape_in.get_state(),tape_in.get_read(),stack_contents.top(),
		SYMBOL_COUNT,
		SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT,
		SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT
	};
	
	D.filter_apply(filter_vals);
}

bool pda::simulate_step_taken(){
	if(D.filter_results() > 0){
		const symb *transition_applied = D.filter_nav_select();
		stack_contents.pop();
		
		for(uint j = 4;j < 12;++j){
			if(transition_applied[j] != SYMBOL_COUNT){
				stack_contents.push(transition_applied[j]);
			}
		}
		
		return tape_in.simulate(transition_applied[3]);
	}else{
		return tape_in.simulate(tape_in.get_state());
	}
}

// -----------

pda::pda():
	automaton<7>(&q0,&D,&S,NULL,&tape_in),
	S(' ','S',&on_set_remove_callback),Q(' ','Q',&on_set_remove_callback),G(' ','G',&on_set_remove_callback),
	D(' ','D'),
	q0('q','0'),g0('g','0'),F(' ','F',NULL)
{
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&G);
	D.set_superset(3,&Q);
	
	for(uint j = 4;j < 12;++j){
		D.set_superset(j,&G);
	}
	
	q0.set_superset(0,&Q);
	g0.set_superset(0,&G);
	F.set_superset(0,&Q);
	
	tape_in.set_superset(&S);
	
	// Interface table population
	interfaces[0] = &S;
	interfaces[1] = &Q;
	interfaces[2] = &G;
	interfaces[3] = &D;
	interfaces[4] = &q0;
	interfaces[5] = &g0;
	interfaces[6] = &F;
}

// ------------------------------------------------------------ ||

tm *tm::current_callback_tm = NULL;

void tm::on_set_remove_callback(const set *s,symb val){
	if(s == &(current_callback_tm->S)){
		current_callback_tm->D.remove_containing(1,val);
		current_callback_tm->D.remove_containing(3,val);
		
		current_callback_tm->s0.remove_containing(0,val);
		
	}else if (s == &(current_callback_tm->Q)){
		current_callback_tm->D.remove_containing(0,val);
		current_callback_tm->D.remove_containing(2,val);
		
		current_callback_tm->q0.remove_containing(0,val);
		current_callback_tm->F.remove_containing(0,val);
	}
}

// -----------

void tm::preupdate(){
	current_callback_tm = this;
}

bool tm::presimulate_check() const{
	return true;
}

void tm::presimulate(){
	// Nothing
}

int tm::postdraw(int y,int x) const{
	return y;
}

bool tm::halt_condition() const{
	return F.contains(tape_in.get_state());
}

void tm::postsimulate(){
	// Nothing
}

void tm::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[5] = {tape_in.get_state(),tape_in.get_read(),SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

bool tm::simulate_step_taken(){
	if(D.filter_results() > 0){
		const symb *transition_applied = D.filter_nav_select();
		ib_tape::motion motion = ib_tape::MOTION_NONE;
		
		if(transition_applied[4] == symbol('L')){
			motion = ib_tape::MOTION_LEFT;
			
		}else if(transition_applied[4] == symbol('R')){
			motion = ib_tape::MOTION_RIGHT;
		}
		
		return tape_in.simulate(transition_applied[2],transition_applied[3],motion);
	}else{
		return tape_in.simulate(tape_in.get_state(),tape_in.get_read(),ib_tape::MOTION_NONE);
	}
}

// -----------

set tm::M(' ','M',NULL);

void tm::init(){
	M.edit('u');
	M.edit('L');
	M.edit('\t');
	M.edit('R');
	M.edit('\t');
	M.edit('N');
	M.edit('\n');
}

tm::tm():
	automaton<6>(&q0,&D,&S,&s0,&tape_in),
	S(' ','S',&on_set_remove_callback),Q(' ','Q',&on_set_remove_callback),D(' ','D'),
	s0(' ','b'),q0('q','0'),F(' ','F',NULL)
{
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&Q);
	D.set_superset(3,&S);
	D.set_superset(4,&M);
	
	s0.set_superset(0,&S);
	q0.set_superset(0,&Q);
	F.set_superset(0,&Q);
	
	tape_in.set_superset(&S);
	
	// Interface table population
	interfaces[0] = &S;
	interfaces[1] = &Q;
	interfaces[2] = &D;
	interfaces[3] = &s0;
	interfaces[4] = &q0;
	interfaces[5] = &F;
}