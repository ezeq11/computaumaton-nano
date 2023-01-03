#ifndef SET_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "bit_array.h"
	
	BIT_ARRAY_DECL(SYMBOL_COUNT)
	
	struct product;
	
	struct set{
		struct set     *superset;
		struct set     *subset;
		struct product *product;
		
		bit_array(SYMBOL_COUNT) members;
	};
	
	#define SET_INIT(SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT) {SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT}
	
	void set_add   (struct set *s,symb i);
	void set_remove(struct set *s,symb i);
	
	bool set_contains(struct set *s,symb i);
	
	#define SET_INCLUDED
#endif