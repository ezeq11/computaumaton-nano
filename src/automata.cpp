#include "curses.hpp"
#include "automata.hpp"

// ------------------------------------------------------------ ||
stack_module::stack_module()
:G(' ','G'),g0('g','0'),stack_contents(){
	// Superset linking
	g0.set_superset(0,&G);
}

// ------------------------------------------------------------ ||
blank_symbol_module::blank_symbol_module()
:s0(' ','b'){
	
}

// ------------------------------------------------------------ ||
bool automaton::simulation_is_selecting() const{
	return D->filter_results() > 1;
}

void automaton::simulation_filter(){
	simulate_step_filter();
	
	if(current_state == STATE_SIMULATING && !simulation_is_selecting()){
		set_timeout(200);
	}else{
		set_timeout(-1);
	}
}

bool automaton::simulation_is_finished() const{
	return blank_symbol_mod == NULL ? tape.at_end() : A.contains(tape.get_state());
}

void automaton::simulation_end(){
	set_timeout(-1);
	D->filter_clear();
}

// -----------
automaton *automaton::current_callback_automaton;

void automaton::on_set_remove_callback(const set *superset,symb to_remove){
	for(uint i = 0;i < current_callback_automaton->interface_count;++i){
		current_callback_automaton->interfaces[i]->remove_containing(superset,to_remove);
	}
}

void (*set_on_remove_callback)(const set *,symb) = &(automaton::on_set_remove_callback); // fulfilling the contract from set.hpp

// -----------
automaton::automaton(stack_module *init_stack_module,blank_symbol_module *init_blank_symbol_module,component_interface *init_transition_table,product_interface *init_D):
	current_state(STATE_IDLE),
	current_focus(0),
	
	#define INTERFACE_MULTIPLEX(null_null,null_blank,stack_null,stack_blank) (\
		init_stack_module == NULL\
		?\
		(init_blank_symbol_module == NULL ? (component_interface *)null_null : (component_interface *)null_blank)\
		:\
		(init_blank_symbol_module == NULL ? (component_interface *)stack_null : (component_interface *)stack_blank)\
	)
	
	interfaces{
		&S,
		&Q,
		init_stack_module == NULL ? init_transition_table : &(init_stack_module->G),
		INTERFACE_MULTIPLEX(&q0 ,&(init_blank_symbol_module->s0),init_transition_table   ,init_transition_table          ),
		INTERFACE_MULTIPLEX(&A  ,&q0                            ,&q0                     ,&(init_blank_symbol_module->s0)),
		INTERFACE_MULTIPLEX(NULL,&A                             ,&(init_stack_module->g0),&q0                            ),
		INTERFACE_MULTIPLEX(NULL,NULL                           ,&A                      ,&(init_stack_module->g0)       ),
		INTERFACE_MULTIPLEX(NULL,NULL                           ,NULL                    ,&A                             ) 
	},
	interface_count(5 + (init_stack_module == NULL ? 0 : 2) + (init_blank_symbol_module == NULL ? 0 : 1)),
	
	stack_mod(init_stack_module),
	blank_symbol_mod(init_blank_symbol_module),
	
	transition_table(init_transition_table),
	D(init_D),
	
	S (' ','S'),
	Q (' ','Q'),
	q0('q','0'),
	A (' ','F'),
	
	tape(&S,init_blank_symbol_module == NULL)
{
	// Superset linking
	q0.set_superset(0,&Q);
	A.set_superset(0,&Q);
}

void automaton::update(int in){
	current_callback_automaton = this;
	
	switch(current_state){
	case STATE_IDLE:
		interfaces[current_focus]->edit(in);
		
		if(interfaces[current_focus]->is_amid_edit()){
			break;
		}
		
		switch(in){
		case 'k':
		case 'j':
			current_focus = (interface_count + current_focus - (in == 'k' ? 1 : 0) + (in == 'j' ? 1 : 0)) % interface_count;
			
			break;
		case ':':
			if(!q0.is_set() || (stack_mod != NULL && !stack_mod->g0.is_set()) || (blank_symbol_mod != NULL && !blank_symbol_mod->s0.is_set())){
				break;
			}
			
			tape.init_edit(blank_symbol_mod == NULL ? SYMBOL_COUNT : blank_symbol_mod->s0.get());
			current_state = STATE_TAPE_INPUT;
			
			break;
		}
		
		break;
	case STATE_TAPE_INPUT:
		switch(in){
		case '`':
			current_state = STATE_IDLE;
			
			break;
		case ' ':
		case '\t':
			tape.init_simulate(q0.get());
			
			if(stack_mod != NULL){
				stack_mod->stack_contents.clear();
				stack_mod->stack_contents.push(stack_mod->g0.get());
			}
			
			if(simulation_is_finished()){
				current_state = STATE_HALTED;
			}else{
				current_state = (in == ' ') ? STATE_STEPPING : STATE_SIMULATING;
				simulation_filter();
			}
			
			break;
		default:
			tape.edit(in);
			
			break;
		}
		
		break;
	case STATE_STEPPING:
	case STATE_SIMULATING:
		if(in == '`'){
			// Escape
			simulation_end();
			
			if(stack_mod != NULL){
				stack_mod->stack_contents.clear();
			}
			
			current_state = STATE_TAPE_INPUT;
			
		}else if(
			(current_state == STATE_STEPPING && in == ' ') ||
			(current_state == STATE_SIMULATING && (!simulation_is_selecting() || in == '\t'))
		){
			// Select current transition
			simulate_step_taken();
			
			if(simulation_is_finished()){
				simulation_end();
				current_state = STATE_HALTED;
				
			}else{
				simulation_filter();
				
			}
		}else if(simulation_is_selecting()){
			// Navigate currently applicable transitions
			switch(in){
			case 'k':
				D->filter_nav_prev();
				
				break;
			case 'j':
				D->filter_nav_next();
				
				break;
			}
		}
		
		break;
	case STATE_HALTED:
		if(in == '\n' || in == '\r'){
			if(stack_mod != NULL){
				stack_mod->stack_contents.clear();
			}
			
			current_state = STATE_TAPE_INPUT;
		}
		
		break;
	}
}

bool automaton::is_interruptible() const{
	return current_state == STATE_IDLE && !interfaces[current_focus]->is_amid_edit();
}

int automaton::draw(int y,int x,bool illustrate_supersets,int commands_x) const{
	// Components
	const component_interface *current_superset = NULL;
	
	if(illustrate_supersets){
		if(current_state == STATE_IDLE && interfaces[current_focus]->is_amid_edit()){
			current_superset = (const component_interface *)interfaces[current_focus]->get_superset_current();
			
		}else if(current_state == STATE_TAPE_INPUT){
			current_superset = (const component_interface *)&S;
		}
	}
	
	for(uint i = 0;i < interface_count;++i){
		if(current_superset == NULL || interfaces[i] == current_superset || (current_state == STATE_IDLE && i == current_focus)){
			if(current_state == STATE_IDLE && i == current_focus){
				move(y,commands_x);
				
				if(!interfaces[current_focus]->is_amid_edit()){
					printw(STRL("[j][k]"));
				}
				
				interfaces[current_focus]->print_available_commands();
			}else if((current_state == STATE_STEPPING || current_state == STATE_HALTED) && interfaces[i] == transition_table && simulation_is_selecting()){
				move(y,commands_x);
				printw(STRL("[j][k]"));
			}
			
			y = interfaces[i]->draw(y,x,(current_state == STATE_IDLE) && (i == current_focus),simulation_is_selecting());
		}else{
			y = interfaces[i]->nodraw(y);
		}
	}
	
	// Tape input
	switch(current_state){
	case STATE_IDLE:
		if(interfaces[current_focus]->is_amid_edit()){
			break;
		}
		
		move(y + 2,commands_x);
		printw(q0.is_set() && (stack_mod == NULL || stack_mod->g0.is_set()) && (blank_symbol_mod == NULL || blank_symbol_mod->s0.is_set()) ? STRL("[:]") : STRL("[ ]"));
		
		break;
	case STATE_TAPE_INPUT:
		move(y + 2,commands_x);
		
		printw(STRL("[`][tab][space]  "));
		tape.print_available_commands();
		
		break;
	case STATE_STEPPING:
	case STATE_SIMULATING:
		move(y + 2,commands_x);
		printw(STRL("[`]"));
		
		if(current_state == STATE_STEPPING){
			printw(STRL("[space]"));
		}else if(simulation_is_selecting()){
			printw(STRL("[tab]"));
		}else{
			printw(STRL("[   ]"));
		}
		
		break;
	case STATE_HALTED:
		move(y + 2,commands_x);
		printw(STRL("[enter]"));
		
		break;
	}
	
	if(current_state != STATE_IDLE){
		y = tape.draw(y,x,!simulation_is_selecting(),current_state != STATE_TAPE_INPUT);
	}else{
		y = tape.nodraw(y);
	}
	
	// Stack contents
	if(stack_mod != NULL){
		if(current_state == STATE_STEPPING || current_state == STATE_SIMULATING || current_state == STATE_HALTED){
			y = stack_mod->stack_contents.draw(y,x);
		}else{
			y = stack_mod->stack_contents.nodraw(y);
		}
	}
	
	// State indicator
	move(y,commands_x);
	
	#ifdef ARDUINO_NANO_BUILD
		printw(STRL("[?]"));
	#else
		printw(STRL("[?][esc]"));
	#endif
	
	move(y,x);
	
	switch(current_state){
	case STATE_IDLE:
		printw(interfaces[current_focus]->is_amid_edit() ? STRL("editing --------------- |") : STRL("idle ------------------ |"));
		break;
	case STATE_TAPE_INPUT:
		printw(STRL("tape input ------------ |"));
		break;
	case STATE_STEPPING:
		printw(STRL("stepping -------------- |"));
		break;
	case STATE_SIMULATING:
		printw(STRL("simulating ------------ |"));
		break;
	case STATE_HALTED:
		printw(A.contains(tape.get_state()) ? STRL("halted - ACCEPTED ----- |") : STRL("halted - REJECTED ----- |"));
		break;
	}
	
	y += 2;
	
	// Done
	return y;
}

// ------------------------------------------------------------ ||
void fsa::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[3] = {tape.get_state(),tape.get_read(),SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

void fsa::simulate_step_taken(){
	tape.simulate(D.filter_results() > 0 ? D.filter_nav_select()[2] : tape.get_state(),tape.get_read(),stateful_tape::MOTION_RIGHT);
}

// -----------
fsa::fsa()
:automaton(NULL,NULL,&D,&D),D(' ','D'){
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&Q);
}

// ------------------------------------------------------------ ||
void pda::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[PDA_TUPLE_LEN];
	filter_vals[0] = tape.get_state();
	filter_vals[1] = tape.get_read();
	filter_vals[2] = stack_mod.stack_contents.top();
	
	for(uint j = 3;j < PDA_TUPLE_LEN;++j){
		filter_vals[j] = SYMBOL_COUNT;
	}
	
	D.filter_apply(filter_vals);
}

void pda::simulate_step_taken(){
	if(D.filter_results() > 0){
		const symb *transition_applied = D.filter_nav_select();
		stack_mod.stack_contents.pop();
		
		for(uint j = 4;j < PDA_TUPLE_LEN;++j){
			if(transition_applied[j] != SYMBOL_COUNT){
				stack_mod.stack_contents.push(transition_applied[j]);
			}
		}
		
		tape.simulate(transition_applied[3],tape.get_read(),stateful_tape::MOTION_RIGHT);
	}else{
		tape.simulate(tape.get_state(),tape.get_read(),stateful_tape::MOTION_RIGHT);
	}
}

// -----------
pda::pda()
:automaton(&stack_mod,NULL,&D,&D),stack_mod(),D(' ','D'){
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&(stack_mod.G));
	D.set_superset(3,&Q);
	
	for(uint j = 4;j < PDA_TUPLE_LEN;++j){
		D.set_superset(j,&(stack_mod.G));
	}
}

// ------------------------------------------------------------ ||
void tm::simulate_step_filter(){
	D.filter_clear();
	
	symb filter_vals[5] = {tape.get_state(),tape.get_read(),SYMBOL_COUNT,SYMBOL_COUNT,SYMBOL_COUNT};
	D.filter_apply(filter_vals);
}

void tm::simulate_step_taken(){
	if(D.filter_results() > 0){
		const symb *transition_applied = D.filter_nav_select();
		stateful_tape::motion motion = stateful_tape::MOTION_NONE;
		
		if(transition_applied[4] == symbol('L')){
			motion = stateful_tape::MOTION_LEFT;
			
		}else if(transition_applied[4] == symbol('R')){
			motion = stateful_tape::MOTION_RIGHT;
		}
		
		tape.simulate(transition_applied[2],transition_applied[3],motion);
	}else{
		tape.simulate(tape.get_state(),tape.get_read(),stateful_tape::MOTION_NONE);
	}
}

set tm::M(' ','M');

// -----------
void tm::init(){
	M.edit('u');
	M.edit('L');
	M.edit('\t');
	M.edit('R');
	M.edit('\t');
	M.edit('N');
	M.edit('\n');
}

tm::tm()
:automaton(NULL,&blank_symbol_mod,&D,&D),blank_symbol_mod(),D(' ','D'){
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	D.set_superset(2,&Q);
	D.set_superset(3,&S);
	D.set_superset(4,&M);
	
	blank_symbol_mod.s0.set_superset(0,&S);
}