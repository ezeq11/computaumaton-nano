#include "compile_config.hpp"
#include "curses.hpp"

#include "automata.hpp"

// ------------------------------------------------------------ ||
stack_module::stack_module(screen_space *G_next,screen_space *g0_next,screen_space *stack_contents_next)
:G(G_next,' ','G'),g0(g0_next,'g','0'),stack_contents(stack_contents_next){
	// Superset linking
	g0.set_superset(0,&G);
}

// ------------------------------------------------------------ ||
blank_symbol_module::blank_symbol_module(screen_space *s0_next)
:s0(s0_next,' ','b'){
	
}

// ------------------------------------------------------------ ||
automaton::status_bar::status_bar()
:screen_space(NULL){
	
}

void automaton::status_bar::demarcate() const{
	screen_space::demarcate(2);
}

void automaton::status_bar::draw(const automaton *subject) const{
	screen_space::clear();
	
	move(screen_space::top(),INDENT_X - 1);
	attrset(A_REVERSE);
	
	switch(subject->current_state){
	case STATE_IDLE:
		printw(subject->tuple_operations.switch_available() ? STRL(" idle                     ") : STRL(" editing                  "));
		break;
	case STATE_TAPE_INPUT:
		printw(STRL(" tape input               "));
		break;
	case STATE_STEPPING:
		printw(STRL(" stepping                 "));
		break;
	case STATE_SIMULATING:
		printw(STRL(" simulating               "));
		break;
	case STATE_HALTED:
		printw(subject->A.contains(subject->tape.get_state()) ? STRL(" halted - ACCEPTED        ") : STRL(" halted - REJECTED        "));
		break;
	}
	
	attrset(A_NORMAL);
	
	switch(subject->current_state){ // see automaton::update()
	case STATE_IDLE:
		if(subject->tuple_operations.switch_available()){
			printw(
				subject->q0.is_set()
				&& (subject->stack_mod == NULL || subject->stack_mod->g0.is_set())
				&& (subject->blank_symbol_mod == NULL || subject->blank_symbol_mod->s0.is_set())
				? STRL(" :")
				: STRL("  ")
			);
			printw(STRL(" j k "));
			
			attrset(A_REVERSE);
			addch(' ');
			attrset(A_NORMAL);
		}
		
		subject->tuple_operations.edit_print_available_commands();
		
		break;
	case STATE_TAPE_INPUT:
		printw(STRL(" ` tab space "));
		
		attrset(A_REVERSE);
		addch(' ');
		attrset(A_NORMAL);
		
		subject->tape.print_available_commands();
		
		break;
	case STATE_STEPPING:
	case STATE_SIMULATING:
		printw(STRL(" ` "));
		
		if(subject->current_state == STATE_STEPPING){
			printw(STRL(" space "));
		}else if(subject->simulation_is_selecting()){
			printw(STRL(" j k tab "));
		}else{
			printw(STRL("         "));
		}
		
		break;
	case STATE_HALTED:
		printw(STRL(" enter "));
		
		break;
	}
	
	attrset(A_REVERSE);
	addch(' ');
	attrset(A_NORMAL);
}

// ------------------------------------------------------------ ||
bool automaton::simulation_is_selecting() const{
	return tuple_operations.filter_results() > 1;
}

void automaton::simulation_filter(){
	tuple_operations.filter_apply(tape.get_state(),tape.get_read(),stack_mod == NULL ? SYMBOL_COUNT : stack_mod->stack_contents.top());
	
	if(current_state == STATE_SIMULATING){
		if(simulation_is_selecting()){
			set_timeout(-1);
			
			tape.draw_overlay_pipe();
		}else{
			set_timeout(200);
		}
	}else{
		set_timeout(-1);
	}
}

bool automaton::simulation_is_finished() const{
	return blank_symbol_mod == NULL ? tape.at_end() : A.contains(tape.get_state());
}

void automaton::simulation_end(){
	set_timeout(-1);
	tuple_operations.filter_clear();
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
	if(superset == &(current_callback_automaton->S)){
		current_callback_automaton->D.remove_containing(superset,to_remove);
		
		if(current_callback_automaton->blank_symbol_mod != NULL){
			current_callback_automaton->blank_symbol_mod->s0.remove_containing(superset,to_remove);
		}
	}
	
	if(superset == &(current_callback_automaton->Q)){
		current_callback_automaton->D.remove_containing(superset,to_remove);
		current_callback_automaton->q0.remove_containing(superset,to_remove);
		current_callback_automaton->A.remove_containing(superset,to_remove);
	}
	
	if(current_callback_automaton->stack_mod != NULL && superset == &(current_callback_automaton->stack_mod->G)){
		current_callback_automaton->D.remove_containing(superset,to_remove);
		current_callback_automaton->stack_mod->g0.remove_containing(superset,to_remove);
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
	
	tuple_operations(),
	
	S (&Q,' ','S'),
	Q (init_stack_module == NULL ? (screen_space *)&D : (screen_space *)&(init_stack_module->G),' ','Q'),
	D (
		init_blank_symbol_module == NULL ? (screen_space *)&q0 : (screen_space *)&(init_blank_symbol_module->s0),
		2 + (init_stack_module == NULL ? 0 : 1),
		1 + (init_blank_symbol_module == NULL ? 0 : 2),
		2 + (init_stack_module == NULL ? 0 : 1) + 1 + (init_blank_symbol_module == NULL ? 0 : 2) + (init_stack_module == NULL ? 0 : STACK_VARIADIC_LEN),
		' ','D'
	),
	q0(init_stack_module == NULL ? (screen_space *)&A : (screen_space *)&(init_stack_module->g0),'q','0'),
	A (&tape,' ','F'),
	
	tape(init_stack_module == NULL ? (screen_space *)&status : (screen_space *)&(init_stack_module->stack_contents),&S,init_blank_symbol_module == NULL),
	status()
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
	tuple_operations.switch_to(interfaces[current_focus],tuple_set_operations::OPERATION_EDIT);
}

void automaton::init_draw(int draw_y) const{
	S.collapse(draw_y);
	
	// Demarcate
	for(uint i = 0;i < interface_count;++i){
		interfaces[i]->demarcate();
	}
	
	tape.demarcate();
	
	if(stack_mod != NULL){
		stack_mod->stack_contents.demarcate();
	}
	
	status.demarcate();
	
	// Draw
	for(uint i = 0;i < interface_count;++i){
		interfaces[i]->draw();
	}
	
	tape.draw();
	
	if(stack_mod != NULL){
		stack_mod->stack_contents.draw();
	}
	
	status.draw(this);
	
	tuple_operations.draw();
}

void automaton::illustrate_supersets(bool illustrate){
	const tuple_set *current_superset = NULL;
	
	if(illustrate){
		if(current_state == STATE_IDLE && !tuple_operations.switch_available()){
			current_superset = tuple_operations.edit_current_superset();
			
		}else if(current_state == STATE_TAPE_INPUT){
			current_superset = &S;
		}
	}
	
	for(uint i = 0;i < interface_count;++i){
		interfaces[i]->draw_set_visibility(current_superset == NULL || interfaces[i] == current_superset || (current_state == STATE_IDLE && i == current_focus));
	}
}

void automaton::update(int in,bool superset_illustration){
	current_callback_automaton = this;
	
	switch(current_state){
	case STATE_IDLE:
		tuple_operations.edit(in);
		
		if(tuple_operations.switch_available()){
			switch(in){
			case 'k':
			case 'j':
				current_focus = (interface_count + current_focus - (in == 'k' ? 1 : 0) + (in == 'j' ? 1 : 0)) % interface_count;
				tuple_operations.switch_to(interfaces[current_focus],tuple_set_operations::OPERATION_EDIT);
				
				break;
			case ':':
				if(!q0.is_set() || (stack_mod != NULL && !stack_mod->g0.is_set()) || (blank_symbol_mod != NULL && !blank_symbol_mod->s0.is_set())){
					break;
				}
				
				tuple_operations.switch_to(&D,tuple_set_operations::OPERATION_FILTER);
				tape.init_edit(blank_symbol_mod == NULL ? SYMBOL_COUNT : blank_symbol_mod->s0.get());
				
				current_state = STATE_TAPE_INPUT;
				
				break;
			}
		}
		
		break;
	case STATE_TAPE_INPUT:
		switch(in){
		case '`':
			tuple_operations.switch_to(interfaces[current_focus],tuple_set_operations::OPERATION_EDIT);
			tape.close();
			
			current_state = STATE_IDLE;
			
			break;
		case ' ':
		case '\t':
			tape.init_simulate(q0.get());
			
			if(stack_mod != NULL){
				stack_mod->stack_contents.clear();
				stack_mod->stack_contents.push(stack_mod->g0.get());
				stack_mod->stack_contents.set_visible(true);
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
				stack_mod->stack_contents.set_visible(false);
			}
			
			tape.init_edit(blank_symbol_mod == NULL ? SYMBOL_COUNT : blank_symbol_mod->s0.get());
			current_state = STATE_TAPE_INPUT;
			
		}else if(
			(current_state == STATE_STEPPING && in == ' ') ||
			(current_state == STATE_SIMULATING && (!simulation_is_selecting() || in == '\t'))
		){
			// Select current transition
			if(tuple_operations.filter_results() > 0){
				const symb *transition_applied = tuple_operations.filter_nav_select();
				
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
				tuple_operations.filter_nav_prev();
				
				break;
			case 'j':
				tuple_operations.filter_nav_next();
				
				break;
			}
		}
		
		break;
	case STATE_HALTED:
		if(in == '\n' || in == '\r'){
			if(stack_mod != NULL){
				stack_mod->stack_contents.clear();
				stack_mod->stack_contents.set_visible(false);
			}
			
			tape.init_edit(blank_symbol_mod == NULL ? SYMBOL_COUNT : blank_symbol_mod->s0.get());
			current_state = STATE_TAPE_INPUT;
		}
		
		break;
	}
	
	illustrate_supersets(superset_illustration);
	status.draw(this);
}

bool automaton::is_interruptible() const{
	return current_state == STATE_IDLE && tuple_operations.switch_available();
}

// ------------------------------------------------------------ ||
fsa::fsa()
:automaton(NULL,NULL){
	// Nothing
}

// ------------------------------------------------------------ ||
pda::pda()
:automaton(&stack_mod,NULL),stack_mod(&D,&A,&status){
	// Nothing
}

// ------------------------------------------------------------ ||
tm::tm()
:automaton(NULL,&blank_symbol_mod),blank_symbol_mod(&q0){
	// Superset linking
	blank_symbol_mod.s0.set_superset(0,&S);
}