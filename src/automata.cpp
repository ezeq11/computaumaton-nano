#include "compile_config.hpp"
#include "curses.hpp"

#include "draw.hpp"
#include "automata.hpp"

// ------------------------------------------------------------ ||
stack_module::stack_module(tuple_set *G_next,tuple_set *g0_next)
:G(G_next,' ','G'),g0(g0_next,'g','0'),stack_contents(){
	// Superset linking
	g0.set_superset(0,&G);
}

// ------------------------------------------------------------ ||
blank_symbol_module::blank_symbol_module(tuple_set *s0_next)
:s0(s0_next,' ','b'){
	
}

// ------------------------------------------------------------ ||
bool automaton::simulation_is_selecting() const{
	return D.filter_results() > 1;
}

void automaton::simulation_filter(){
	D.filter_apply(tape.get_state(),tape.get_read(),stack_mod == NULL ? stack_mod->stack_contents.top() : SYMBOL_COUNT);
	
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
	D.filter_clear();
}

// -----------
set automaton::M(NULL,' ','M');

void automaton::init(){
	symb M_contents[3] = {'L','R','N'};
	
	for(uint i = 0;i < 3;++i){
		M_contents[i] = symbol(M_contents[i]);
		M.on_add(M_contents + i);
	}
}

automaton *automaton::current_callback_automaton;

void automaton::on_set_remove_callback(const tuple_set *superset,symb to_remove){
	for(uint i = 0;i < current_callback_automaton->interface_count;++i){
		current_callback_automaton->interfaces[i]->remove_containing(superset,to_remove);
	}
}

void (*monad_set_on_remove_callback)(const tuple_set *,symb) = &(automaton::on_set_remove_callback); // fulfilling the contract from set.hpp

// -----------
automaton::automaton(stack_module *init_stack_module,blank_symbol_module *init_blank_symbol_module):
	stack_mod(init_stack_module),
	blank_symbol_mod(init_blank_symbol_module),
	
	#define INTERFACE_MULTIPLEX(null_null,null_blank,stack_null,stack_blank) (\
		init_stack_module == NULL\
		?\
		(init_blank_symbol_module == NULL ? (tuple_set *)null_null : (tuple_set *)null_blank)\
		:\
		(init_blank_symbol_module == NULL ? (tuple_set *)stack_null : (tuple_set *)stack_blank)\
	)
	
	interfaces{
		&S,
		&Q,
		init_stack_module == NULL ? (tuple_set *)&D : (tuple_set *)&(init_stack_module->G),
		INTERFACE_MULTIPLEX(&q0 ,&(init_blank_symbol_module->s0),&D                      ,&D                             ),
		INTERFACE_MULTIPLEX(&A  ,&q0                            ,&q0                     ,&(init_blank_symbol_module->s0)),
		INTERFACE_MULTIPLEX(NULL,&A                             ,&(init_stack_module->g0),&q0                            ),
		INTERFACE_MULTIPLEX(NULL,NULL                           ,&A                      ,&(init_stack_module->g0)       ),
		INTERFACE_MULTIPLEX(NULL,NULL                           ,NULL                    ,&A                             ) 
	},
	interface_count(5 + (init_stack_module == NULL ? 0 : 2) + (init_blank_symbol_module == NULL ? 0 : 1)),
	
	current_state(STATE_IDLE),
	current_focus(0),
	
	editor(),
	
	S (&Q,' ','S'),
	Q (init_stack_module == NULL ? (tuple_set *)&D : (tuple_set *)&(init_stack_module->G),' ','Q'),
	D (
		init_blank_symbol_module == NULL ? (tuple_set *)&q0 : (tuple_set *)&(init_blank_symbol_module->s0),
		2 + (init_stack_module == NULL ? 0 : 1),
		1 + (init_blank_symbol_module == NULL ? 0 : 2),
		2 + (init_stack_module == NULL ? 0 : 1) + 1 + (init_blank_symbol_module == NULL ? 0 : 2) + (init_stack_module == NULL ? 0 : STACK_VARIADIC_LEN),
		' ','D'
	),
	q0(init_stack_module == NULL ? (tuple_set *)&A : (tuple_set *)&(init_stack_module->g0),'q','0'),
	A (NULL,' ','F'),
	
	tape(&S,init_blank_symbol_module == NULL)
{
	// Superset linking
	D.set_superset(0,&Q);
	D.set_superset(1,&S);
	
	uint j = 2;
	
	if(init_stack_module != NULL){
		D.set_superset(j,&(init_stack_module->G));
		++j;
	}
	
	D.set_superset(j,&Q);
	++j;
	
	if(init_blank_symbol_module != NULL){
		D.set_superset(j,&S);
		++j;
		
		D.set_superset(j,&M);
		++j;
	}
	
	if(init_stack_module != NULL){
		for(uint k = 0;k < STACK_VARIADIC_LEN;++k){
			D.set_superset(j,&(init_stack_module->G));
			++j;
		}
	}
	
	q0.set_superset(0,&Q);
	A.set_superset(0,&Q);
	
	// Editor initialization
	editor.switch_to(interfaces[current_focus]);
}

void automaton::init_draw(int draw_y) const{
	for(uint i = 0;i < interface_count;++i){
		interfaces[i]->init_draw();
	}
}

void automaton::update(int in,bool illustrate_supersets){
	current_callback_automaton = this;
	
	switch(current_state){
	case STATE_IDLE:
		editor.edit(in);
		
		if(editor.is_amid_edit()){
			break;
		}
		
		switch(in){
		case 'k':
		case 'j':
			current_focus = (interface_count + current_focus - (in == 'k' ? 1 : 0) + (in == 'j' ? 1 : 0)) % interface_count;
			editor.switch_to(interfaces[current_focus]);
			
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
			if(D.filter_results() > 0){
				const symb *transition_applied = D.filter_nav_select();
				
				if(stack_mod != NULL){
					const symb *stack_out = transition_applied + 3 + 1 + (blank_symbol_mod == NULL ? 0 : 2);
					stack_mod->stack_contents.pop();
					
					for(uint k = 0;k < STACK_VARIADIC_LEN;++k){
						if(stack_out[k] != SYMBOL_COUNT){
							stack_mod->stack_contents.push(stack_out[k]);
						}
					}
				}
				
				symb write = tape.get_read();
				stateful_tape::motion motion = stateful_tape::MOTION_RIGHT;
				
				if(blank_symbol_mod != NULL){
					write = transition_applied[2 + (stack_mod == NULL ? 0 : 1) + 1 + 0];
					motion = stateful_tape::MOTION_NONE;
					
					symb motion_symb = transition_applied[2 + (stack_mod == NULL ? 0 : 1) + 1 + 0];
					
					if(motion_symb == symbol('L')){
						motion = stateful_tape::MOTION_LEFT;
						
					}else if(motion_symb == symbol('R')){
						motion = stateful_tape::MOTION_RIGHT;
						
					}
				}
				
				tape.simulate(transition_applied[2 + (stack_mod == NULL ? 0 : 1)],write,motion);
			}else{
				tape.simulate(tape.get_state(),tape.get_read(),blank_symbol_mod == NULL ? stateful_tape::MOTION_RIGHT : stateful_tape::MOTION_NONE);
			}
			
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
				D.filter_nav_prev();
				
				break;
			case 'j':
				D.filter_nav_next();
				
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
	
	// Update component visibility as necessary
	const tuple_set *current_superset = NULL;
	
	if(illustrate_supersets){
		if(current_state == STATE_IDLE && editor.is_amid_edit()){
			current_superset = editor.get_superset_current();
			
		}else if(current_state == STATE_TAPE_INPUT){
			current_superset = &S;
		}
	}
	
	for(uint i = 0;i < interface_count;++i){
		interfaces[i]->set_visibility(current_superset == NULL || interfaces[i] == current_superset || (current_state == STATE_IDLE && i == current_focus));
	}
}

bool automaton::is_interruptible() const{
	return current_state == STATE_IDLE && !editor.is_amid_edit();
}

int automaton::draw(int y,int x){
	y += 20;
	
	// Tape input
	switch(current_state){
	case STATE_IDLE:
		if(editor.is_amid_edit()){
			break;
		}
		
		move(y + 2,COMMANDS_X);
		printw(q0.is_set() && (stack_mod == NULL || stack_mod->g0.is_set()) && (blank_symbol_mod == NULL || blank_symbol_mod->s0.is_set()) ? STRL("[:]") : STRL("[ ]"));
		
		break;
	case STATE_TAPE_INPUT:
		move(y + 2,COMMANDS_X);
		
		printw(STRL("[`][tab][space]  "));
		tape.print_available_commands();
		
		break;
	case STATE_STEPPING:
	case STATE_SIMULATING:
		move(y + 2,COMMANDS_X);
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
		move(y + 2,COMMANDS_X);
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
	move(y,COMMANDS_X);
	
	#ifdef ARDUINO_NANO_BUILD
		printw(STRL("[?]"));
	#else
		printw(STRL("[?][esc]"));
	#endif
	
	move(y,x);
	
	switch(current_state){
	case STATE_IDLE:
		printw(editor.is_amid_edit() ? STRL("editing --------------- |") : STRL("idle ------------------ |"));
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
fsa::fsa()
:automaton(NULL,NULL){
	// Nothing
}

// ------------------------------------------------------------ ||
pda::pda()
:automaton(&stack_mod,NULL),stack_mod(&D,&A){
	// Nothing
}

// ------------------------------------------------------------ ||
tm::tm()
:automaton(NULL,&blank_symbol_mod),blank_symbol_mod(&q0){
	// Superset linking
	blank_symbol_mod.s0.set_superset(0,&S);
}