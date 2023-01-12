#ifndef SET_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "link.h"
	#include "bit_array.h"
	
	extern const struct chain_type SET_TYPE;
	#define LINK_HEAD_INIT_SET(NEXT) LINK_HEAD_INIT(&SET_TYPE,1,1,false,true,false,W_SIZE,NULL)
	
	BIT_ARRAY_DECL(SYMBOL_COUNT)
	
	typedef bit_array(SYMBOL_COUNT) set;
	#define SET_INIT BIT_ARRAY_INIT
	
	// set interface
	// all (void *) arguments are cast to (set *) before performing operations
	uint set_size    (const void *s);
	symb set_get     (const void *s,uint i);
	bool set_contains(const void *s,symb i);
	
	void set_add   (void *s,symb i);
	void set_remove(void *s,symb i);
	
	#define SET_INCLUDED
#endif