#ifndef SET_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "bit_array.hpp"
	#include "component.hpp"
	
	#define SET_COMPONENT                    \
		component<                           \
			1,1,                             \
			true,true,false,false,           \
			false,false,true,16,SYMBOL_COUNT \
		>
	
	class set: public SET_COMPONENT{
		private:
			bit_array<SYMBOL_COUNT>       members;
			bit_array<SYMBOL_COUNT>::iter members_iter;
			
			void (*on_remove_callback)(const set *,symb);
			
		public:
			set(char name_1,char name_2,void (*remove_callback)(const set *,symb));
			
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
			
			// Specialized ------------------------------------------------ ||
			bool contains(symb val) const;
	};
	
	#define SET_INCLUDED
#endif