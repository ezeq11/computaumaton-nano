#ifndef SET_INCLUDED
#define SET_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "bit_array.hpp"
	#include "component.hpp"
	
	#define SET_COMPONENT \
		component<1,1,true,true,false,false>
	
	// declared here, but not defined in set.cpp
	// whoever uses set.edit() (in this case, automata.cpp) has the responsibility of defining its value
	extern void (*set_on_remove_callback)(const set *,symb);
	
	class set: public SET_COMPONENT{
		private:
			bit_array<SYMBOL_COUNT>       members;
			bit_array<SYMBOL_COUNT>::iter members_iter;
			
		public:
			set(char name_1,char name_2);
			
			// Edit ------------------------------------------------------- ||
			virtual void on_add();
			virtual void on_remove();
			virtual void on_set();
			virtual void on_clear();
			
			virtual void remove_containing(const set *superset,symb to_remove);
			
			// Draw ------------------------------------------------------- ||
			virtual int draw_contents(int y,int x,int *cx,bool indicate_current_item) const;
			virtual void draw_buffer(int *cx) const;
			
			virtual int nodraw_contents(int y) const;
			
			// Specialized ------------------------------------------------ ||
			bool contains(symb val) const;
	};
	
#endif