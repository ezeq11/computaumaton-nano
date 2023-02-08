#ifndef AUTOMATON_INCLUDED
	#include "component.hpp"
	
	#include "set.hpp"
	#include "element.hpp"
	#include "product.hpp"
	
	#include "stateful_tape.hpp"
	#include "stack.hpp"
	
	// ------------------------------------------------------------ ||
	struct stack_module{
		set G;
		element g0;
		stack stack_contents;
		
		stack_module();
	};
	
	// ------------------------------------------------------------ ||
	struct blank_symbol_module{
		element s0;
		
		blank_symbol_module();
	};
	
	// ------------------------------------------------------------ ||
	extern int current_delay;
	
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
		
		component_interface * const interfaces[8];
		const uint interface_count;
		
		stack_module * const stack_mod;
		blank_symbol_module * const blank_symbol_mod;
		
		component_interface * const transition_table;
		product_interface * const D;
		
	protected:
		set S;
		set Q;
		element q0;
		set F;
		
		stateful_tape tape;
		
	private:
		void simulating_timeout(int delay) const;
		
		bool simulation_is_selecting() const;
		void simulation_filter();
		void simulation_end();
		
		virtual void simulate_step_filter() = 0;
		virtual void simulate_step_taken() = 0;
		
		static automaton *current_callback_automaton;
		static void on_set_remove_callback(const set *superset,symb to_remove);
		
	public:
		automaton(stack_module *init_stack_module,blank_symbol_module *init_blank_symbol_module,component_interface *init_transition_table,product_interface *init_D);
		
		void update(int in);
		int draw(int y,int x) const;
	};
	
	// ------------------------------------------------------------ ||
	class fsa: public automaton{
	private:
		product<3,3> D;
		
		virtual void simulate_step_filter();
		virtual void simulate_step_taken();
		
	public:
		fsa();
	};
	
	// ------------------------------------------------------------ ||
	class pda: public automaton{
	private:
		stack_module stack_mod;
		product<4,12> D;
		
		virtual void simulate_step_filter();
		virtual void simulate_step_taken();
		
	public:
		pda();
	};
	
	// ------------------------------------------------------------ ||
	class tm: public automaton{
	private:
		blank_symbol_module blank_symbol_mod;
		product<5,5> D;
		
		virtual void simulate_step_filter();
		virtual void simulate_step_taken();
		
		static set M;
	public:
		static void init();
		
		tm();
	};
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif