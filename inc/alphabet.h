#ifndef ALPHABET_INCLUDED
	#include "unsigned.h"
	
	#include "charset.h"
	#include "bit_array.h"
	
	BIT_ARRAY_DECL(CHARSET_SIZE)
	
	struct product;
	
	struct alphabet{
		struct alphabet *superset;
		struct alphabet *subset;
		struct product  *product;
		
		bit_array(CHARSET_SIZE) members;
	};
	
	#define ALPHABET_INIT(SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT) {SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT}
	
	void alphabet_add(struct alphabet *a,uint i);
	void alphabet_remove(struct alphabet *a,uint i);
	
	#define ALPHABET_INCLUDED
#endif