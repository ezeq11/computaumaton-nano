#ifndef AUTOMATON_INCLUDED
	#include "tuple_set.hpp"
	#include "stateful_tape.hpp"
	#include "stack.hpp"
	
	// ------------------------------------------------------------ ||
	struct stack_module{
		set G;
		element g0;
		stack stack_contents;
		
		stack_module(tuple_set *G_next,tuple_set *g0_next);
	};
	
	// ------------------------------------------------------------ ||
	struct blank_symbol_module{
		element s0;
		
		blank_symbol_module(tuple_set *s0_next);
	};
	
	// ------------------------------------------------------------ ||
	#define STACK_VARIADIC_LEN 8
	
	class automaton{
	protected:
		enum state{
			STATE_IDLE,
			STATE_TAPE_INPUT,
			STATE_STEPPING,
			STATE_SIMULATING,
			STATE_HALTED
		};
		
		// -----
		stack_module * const stack_mod;
		blank_symbol_module * const blank_symbol_mod;
		
		tuple_set * const interfaces[8];
		const uint interface_count;
		
		state current_state;
		uint current_focus;
		
		tuple_set_editor editor;
		
		// -----
		set S;
		set Q;
		product D;
		element q0;
		set A;
		
		stateful_tape tape;
		
		// -----
		bool simulation_is_selecting() const;
		void simulation_filter();
		bool simulation_is_finished() const;
		void simulation_end();
		
	public:
		static set M;
		static void init();
		
		static automaton *current_callback_automaton;
		static void on_set_remove_callback(const tuple_set *superset,symb to_remove);
		
		automaton(stack_module *init_stack_module,blank_symbol_module *init_blank_symbol_module);
		
		void init_draw(int draw_y) const;
		
		void update(int in,bool illustrate_supersets);
		bool is_interruptible() const;
		
		int draw(int y,int x);
	};
	
	// ------------------------------------------------------------ ||
	class fsa: public automaton{
	public:
		fsa();
	};
	
	// ------------------------------------------------------------ ||
	class pda: public automaton{
	private:
		stack_module stack_mod;
		
	public:
		pda();
	};
	
	// ------------------------------------------------------------ ||
	class tm: public automaton{
	private:
		blank_symbol_module blank_symbol_mod;
		
	public:
		tm();
	};
	
	#define AUTOMATON_INCLUDED
#endif