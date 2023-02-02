#ifndef AUTOMATON_INCLUDED
	#include "component.hpp"
	
	#include "set.hpp"
	#include "element.hpp"
	#include "product.hpp"
	
	#define AUTOMATON_PRODUCT_BLOCK_SIZE 128
	
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
			focus current_focus;
			component_interface * const interfaces[FOCUS_COUNT];
			
			set S;
			set Q;
			element q0;
			product<3,3,AUTOMATON_PRODUCT_BLOCK_SIZE> D;
			set F;
			
		public:
			fsa();
			
			void update(int in);
			int draw(int y,int x) const;
	};
	
	#define AUTOMATON_INCLUDED
#endif