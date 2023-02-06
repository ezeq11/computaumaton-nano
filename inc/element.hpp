#ifndef ELEMENT_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "component.hpp"
	
	#define ELEMENT_COMPONENT \
		component<1,1,false,false,true,true>
	
	class element: public ELEMENT_COMPONENT{
		private:
			symb val;
			
		public:
			element(char name_1,char name_2);
			
			// Edit ------------------------------------------------------- ||
			virtual void on_add();
			virtual void on_remove();
			virtual void on_set();
			virtual void on_clear();
			
			virtual void remove_containing(uint j,symb to_remove);
			
			// Draw ------------------------------------------------------- ||
			virtual int draw_contents(int y,int x,int *cx,bool indicate_current_item) const;
			virtual void draw_buffer(int *cx) const;
			
			virtual int nodraw_contents(int y) const;
			
			// Specialized ------------------------------------------------ ||
			bool is_set() const;
			symb get() const;
	};
	
	#define ELEMENT_INCLUDED
#endif