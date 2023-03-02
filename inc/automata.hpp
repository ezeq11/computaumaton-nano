#ifndef AUTOMATON_INCLUDED
	#include "compile_config.hpp"
	
	#include "screen_space.hpp"
	
	#include "tuple_set.hpp"
	#include "stateful_tape.hpp"
	#include "stack.hpp"
	
	// ------------------------------------------------------------ ||
	struct stack_module{
		set G;
		element g0;
		stack stack_contents;
		
		stack_module(screen_space *G_next,screen_space *g0_next,screen_space *stack_contents_next);
	};
	
	// ------------------------------------------------------------ ||
	struct blank_symbol_module{
		element s0;
		
		blank_symbol_module(screen_space *s0_next);
	};
	
	// ------------------------------------------------------------ ||
	#ifdef ARDUNI_NANO_BUILD
		#define STACK_VARIADIC_LEN 4
	#else
		#define STACK_VARIADIC_LEN 8
	#endif
	
	class automaton{
	protected:
		enum state{
			STATE_IDLE,
			STATE_TAPE_INPUT,
			STATE_STEPPING,
			STATE_SIMULATING,
			STATE_HALTED
		};
		
		class status_bar: public screen_space{
		public:
			status_bar();
			
			void demarcate() const;
			void draw(const automaton *subject) const;
		};
		
		// -----
		stack_module * const stack_mod;
		blank_symbol_module * const blank_symbol_mod;
		
		tuple_set * const interfaces[8];
		const uint interface_count;
		
		state current_state;
		uint current_focus;
		
		tuple_set_operations tuple_operations;
		
		// -----
		set S;
		set Q;
		product D;
		element q0;
		set A;
		
		stateful_tape tape;
		status_bar status;
		
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
		void illustrate_supersets(bool illustrate);
		
		void update(int in,bool superset_illustration);
		bool is_interruptible() const;
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