#ifndef ELEMENT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "link.h"
	
	extern const struct chain_type ELEMENT_TYPE;
	
	typedef symb element;
	#define ELEMENT_INIT SYMBOL_COUNT
	
	// element interface ------------------------------------------ ||
	// all (void *) arguments are cast to (element *) before performing operations
	uint element_size    (const void *e);
	bool element_contains(const void *e,symb i);
	
	void element_set  (void *e,symb val);
	void element_clear(void *e);
	
	symb element_get(const void *e,uint i);
	
	#define ELEMENT_INCLUDED
#endif