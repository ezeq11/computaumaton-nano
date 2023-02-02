#ifndef ELEMENT_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "component.hpp"
	
	#define ELEMENT_COMPONENT      \
		component<                 \
			1,1,                   \
			false,false,true,true, \
			true,false,false,1,1   \
		>
	
	class element: public ELEMENT_COMPONENT{
		private:
			symb val;
			
		public:
			element(char name_1,char name_2);
			
			// Edit ------------------------------------------------------- ||
			virtual uint size() const;
			
			virtual void on_add();
			virtual void on_remove();
			virtual void on_set();
			virtual void on_clear();
			
			// Draw ------------------------------------------------------- ||
			virtual bool horizontal_iter_begin() const;
			virtual bool horizontal_iter_seek() const;
			
			virtual bool horizontal_iter_is_current() const;
			virtual bool horizontal_iter_is_visible() const;
			virtual symb horizontal_iter_get(uint j) const;
			
			virtual bool vertical_is_current(uint i) const;
			virtual bool vertical_is_visible(uint i) const;
			virtual symb vertical_get(uint i,uint j) const;
	};
	
	#define ELEMENT_INCLUDED
#endif