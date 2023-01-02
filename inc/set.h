#ifndef SET_INCLUDED
	#include "unsigned.h"
	
	#include "charset.h"
	#include "bit_array.h"
	
	BIT_ARRAY_DECL(CHARSET_SIZE)
	
	struct product;
	
	struct set{
		struct set     *superset;
		struct set     *subset;
		struct product *product;
		
		bit_array(CHARSET_SIZE) members;
	};
	
	#define SET_INIT(SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT) {SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT}
	
	void set_add(struct set *s,uint i);
	void set_remove(struct set *s,uint i);
	
	#define SET_INCLUDED
#endif