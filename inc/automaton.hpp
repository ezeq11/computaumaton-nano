#ifndef AUTOMATON_INCLUDED
	#include "component.hpp"
	
	#include "set.hpp"
	#include "element.hpp"
	#include "product.hpp"
	
	#include "tapes.hpp"
	
	#define AUTOMATON_PRODUCT_BLOCK_SIZE 128
	
	extern int current_delay;
	
	template <uint NUM_COMPONENTS>
	class automaton{
		private:
			enum state{
				STATE_IDLE,
				STATE_TAPE_INPUT,
				STATE_STEPPING,
				STATE_SIMULATING,
				STATE_HALTED
			};
			
			state current_state;
			uint current_focus;
			
			void simulating_timeout(int delay) const;
			
			void simulation_filter();
			bool simulation_selecting() const;
			void simulation_end(state new_state);
			
		protected:
			element * const initial_state;
			product_interface * const transition_table;
			
			set * const in_alphabet;
			element * const blank_symbol;
			tape * const input;
			
			component_interface * interfaces[NUM_COMPONENTS];
			
			virtual void preupdate() = 0;
			virtual bool presimulate_check() const = 0;
			virtual void presimulate() = 0;
			
			virtual void simulate_step_filter() = 0;
			virtual bool simulate_step_taken() = 0;
			
		public:
			automaton(element *new_initial_state,product_interface *new_transition_table,set *new_in_alphabet,element *new_blank_symbol,tape *new_input);
			
			void update(int in);
			int draw(int y,int x) const;
	};
	
	class fsa: public automaton<5>{
		private:
			set S;
			set Q;
			element q0;
			product<3,3,AUTOMATON_PRODUCT_BLOCK_SIZE> D;
			set F;
			
			fu_tape tape_in;
			
			static fsa *current_callback_fsa;
			static void on_set_remove_callback(const set *s,symb val);
			
		protected:
			virtual void preupdate();
			virtual bool presimulate_check() const;
			virtual void presimulate();
			
			virtual void simulate_step_filter();
			virtual bool simulate_step_taken();
			
		public:
			fsa();
	};
	
	// ------------------------------------------------------------ ||
	
	template<uint NUM_COMPONENTS>
	void automaton<NUM_COMPONENTS>::simulating_timeout(int delay) const{
		if(current_state != STATE_SIMULATING){
			return;
		}
		
		if(delay == current_delay){
			return;
		}
		
		timeout(delay);
		current_delay = delay;
	}
	
	template<uint NUM_COMPONENTS>
	void automaton<NUM_COMPONENTS>::simulation_filter(){
		simulate_step_filter();
		
		if(simulation_selecting()){
			simulating_timeout(-1);
		}else{
			simulating_timeout(200);
		}
	}
	
	template<uint NUM_COMPONENTS>
	bool automaton<NUM_COMPONENTS>::simulation_selecting() const{
		return transition_table->filter_results() > 1;
	}
	
	template<uint NUM_COMPONENTS>
	void automaton<NUM_COMPONENTS>::simulation_end(state new_state){
		simulating_timeout(-1);
		transition_table->filter_clear();
		
		current_state = new_state;
	}
	
	// -----------
	
	template<uint NUM_COMPONENTS>
	automaton<NUM_COMPONENTS>::automaton(element *new_initial_state,product_interface *new_transition_table,set *new_in_alphabet,element *new_blank_symbol,tape *new_input):
		current_state(STATE_IDLE),current_focus(0),
		initial_state(new_initial_state),transition_table(new_transition_table),in_alphabet(new_in_alphabet),blank_symbol(new_blank_symbol),input(new_input)
	{
		
	}
	
	template<uint NUM_COMPONENTS>
	void automaton<NUM_COMPONENTS>::update(int in){
		preupdate();
		
		switch(current_state){
		case STATE_IDLE:
			interfaces[current_focus]->edit(in);
			
			if(interfaces[current_focus]->is_amid_edit()){
				break;
			}
			
			switch(in){
			case KEY_UP:
			case KEY_DOWN:
				current_focus = (NUM_COMPONENTS + current_focus - (in == KEY_UP) + (in == KEY_DOWN)) % NUM_COMPONENTS;
				
				break;
			case ':':
				if(!initial_state->is_set() || (blank_symbol != NULL && !blank_symbol->is_set()) || !presimulate_check()){
					break;
				}
				
				input->init_edit(blank_symbol == NULL ? SYMBOL_COUNT : blank_symbol->get());
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
				if(!input->can_simulate()){
					break;
				}
				
				current_state = (in == ' ') ? STATE_STEPPING : STATE_SIMULATING;
				
				input->init_simulate(initial_state->get());
				presimulate();
				simulation_filter();
				
				break;
			default:
				input->edit(in);
				
				break;
			}
			
			break;
		case STATE_STEPPING:
		case STATE_SIMULATING:
			if(in == '`'){
				// Escape
				simulation_end(STATE_TAPE_INPUT);
				
			}else if(
				(current_state == STATE_STEPPING && in == ' ') ||
				(current_state == STATE_SIMULATING && (!simulation_selecting() || in == '\t'))
			){
				// Select current transition
				if(simulate_step_taken()){
					simulation_end(STATE_HALTED);
					
				}else{
					simulation_filter();
					
				}
			}else if(simulation_selecting()){
				// Navigate currently applicable transitions
				switch(in){
				case KEY_UP:
					transition_table->filter_nav_prev();
					
					break;
				case KEY_DOWN:
					transition_table->filter_nav_next();
					
					break;
				}
			}
			
			break;
		case STATE_HALTED:
			if(in == '\n'){
				current_state = STATE_TAPE_INPUT;
			}
			
			break;
		}
	}
	
	template<uint NUM_COMPONENTS>
	int automaton<NUM_COMPONENTS>::draw(int y,int x) const{
		// Components
		const component_interface *current_superset = NULL;
		
		if(current_state == STATE_IDLE && interfaces[current_focus]->is_amid_edit()){
			current_superset = (const component_interface *)interfaces[current_focus]->get_superset_current();
			
		}else if(current_state == STATE_TAPE_INPUT){
			current_superset = (const component_interface *)in_alphabet;
		}
		
		for(uint i = 0;i < NUM_COMPONENTS;++i){
			if(current_superset == NULL || interfaces[i] == current_superset || (current_state == STATE_IDLE && i == current_focus)){
				y = interfaces[i]->draw(y,x,(current_state == STATE_IDLE) && (i == current_focus),simulation_selecting());
			}else{
				y = interfaces[i]->nodraw(y);
			}
		}
		
		// Tape input
		if(current_state != STATE_IDLE){
			y = input->draw(y,x,!simulation_selecting(),current_state != STATE_TAPE_INPUT);
		}else{
			y = input->nodraw(y);
		}
		
		// Available commands
		move(y,x);
		printw("|--- esc --- ");
		
		switch(current_state){
		case STATE_IDLE:
			if(!interfaces[current_focus]->is_amid_edit()){
				printw(initial_state->is_set() && (blank_symbol == NULL || blank_symbol->is_set()) && presimulate_check() ? ": " : "# ");
			}
			
			interfaces[current_focus]->print_available_commands();
			
			if(!interfaces[current_focus]->is_amid_edit()){
				printw("--- up down ---| idle");
			}
			
			break;
		case STATE_TAPE_INPUT:
			printw(input->can_simulate() ? "` tab space --- " : "` ### ##### --- ");
			input->print_available_commands();
			printw("---| tape input");
			
			break;
		case STATE_STEPPING:
		case STATE_SIMULATING:
			printw("` ");
			
			if(current_state == STATE_STEPPING){
				printw("space --- ");
			}else if(simulation_selecting()){
				printw("tab --- ");
			}else{
				printw("### --- ");
			}
			
			printw(simulation_selecting() ? "up down " : "## #### ");
			printw(current_state == STATE_STEPPING ? "---| stepping" : "---| simulating");
			
			break;
		case STATE_HALTED:
			printw("enter ---| halted");
			
			break;
		}
		
		y += 2;
		
		// Done
		return y;
	}
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif