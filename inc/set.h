#ifndef SET_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "link.h"
	#include "bit_array.h"
	
	extern const struct chain_type SET_TYPE;
	
	BIT_ARRAY_DECL(SYMBOL_COUNT)
	typedef bit_array(SYMBOL_COUNT) set;
	#define SET_INIT BIT_ARRAY_INIT
	
	// set interface ---------------------------------------------- ||
	// all (void *) arguments are cast to (set *) before performing operations
	uint set_size    (const void *s);
	bool set_contains(const void *s,symb val);
	
	void set_add   (void *s,symb val);
	void set_remove(void *s,symb val);
	
	// ------------------------------------------------------------ ||
	
	bool set_iter_begin(const void *s);
	bool set_iter_seek (const void *s);
	symb set_iter_get  (const void *s);
	
	#define SET_INCLUDED
#endif