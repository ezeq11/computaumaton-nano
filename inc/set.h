#ifndef SET_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "bit_array.h"
	#include "link.h"
	
	extern const struct link_funcset SET_FUNCSET;
	#define LINK_HEAD_INIT_SET(NEXT) LINK_HEAD_INIT(&SET_FUNCSET,(NEXT))
	
	BIT_ARRAY_DECL(SYMBOL_COUNT)
	typedef bit_array(SYMBOL_COUNT) set;
	
	#define SET_INIT {0} // all zeroes
	
	// set interface
	// all arguments are cast to (set *) before performing operations
	void set_add   (void *s,symb i);
	void set_remove(void *s,symb i);
	
	bool set_contains(const void *s,symb i);
	
	#define SET_INCLUDED
#endif