#ifndef AUTOMATON_INCLUDED
	#include "component.hpp"
	
	#include "set.hpp"
	#include "element.hpp"
	#include "product.hpp"
	
	#include "tapes.hpp"
	
	#define AUTOMATON_PRODUCT_BLOCK_SIZE 128
	
	enum automaton_state{
		AUT_STATE_IDLE,
		AUT_STATE_TAPE_INPUT,
		AUT_STATE_STEPPING,
		AUT_STATE_SIMULATING,
		AUT_STATE_HALTED
	};
	
	class fsa{
		public:
			enum focus{
				FOCUS_S,
				FOCUS_Q,
				FOCUS_Q0,
				FOCUS_D,
				FOCUS_F,
				
				FOCUS_COUNT
			};
			
		private:
			automaton_state state;
			focus current_focus;
			
			component_interface * const interfaces[FOCUS_COUNT];
			set S;
			set Q;
			element q0;
			product<3,3,AUTOMATON_PRODUCT_BLOCK_SIZE> D;
			set F;
			
			fu_tape tape_in;
			
			void simulating_timeout(int delay) const;
			
			void simulate_step_filter();
			bool simulate_step_taken();
			
			void simulation_filter();
			bool simulation_selecting() const;
			void simulation_end(automaton_state new_state);
			
		public:
			fsa();
			
			void update(int in);
			int draw(int y,int x) const;
	};
	
	#define AUTOMATON_INCLUDED
#endif