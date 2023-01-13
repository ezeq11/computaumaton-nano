#ifndef PRODUCT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "link.h"
	#include "symb_list.h"
	
	extern const struct chain_type PRODUCT_TYPE;
	
	typedef struct symb_list product;
	#define PRODUCT_INIT SYMB_LIST_INIT
	
	// product interface ------------------------------------------ ||
	// all (void *) arguments are cast to (product *) before performing operations
	uint product_size(const void *p);
	bool product_contains(const void *p,symb val);
	
	// ---
	void product_add_init();
	void product_add(void *p,symb val);
	void product_add_complete(void *p,symb val);
	
	// ---
	void product_remove_init();
	
	void product_and_remove(void *p,symb val);
	void product_and_remove_complete(void *p,symb val);
	
	void product_or_remove(void *p,symb val);
	void product_or_remove_complete(void *p,symb val);
	
	// ---
	symb product_get(const void *p,uint i);
	
	#define PRODUCT_INCLUDED
#endif