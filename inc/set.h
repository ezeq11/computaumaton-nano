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
		
		bool q_element_written;
		symb q_element;
		
		bit_array(SYMBOL_COUNT) members;
	};
	
	#define SET_INIT(SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT) {SUPERSET_INIT,SUBSET_INIT,PRODUCT_INIT,0,SYMBOL_COUNT}
	
	void set_q_clear  (struct set *s);
	void set_q_enqueue(struct set *s,symb val);
	void set_q_dequeue(struct set *s);
	symb set_q_get    (struct set *s);
	
	bool set_q_add   (struct set *s);
	bool set_q_remove(struct set *s);
	
	void set_add   (struct set *s,symb i);
	void set_remove(struct set *s,symb i);
	
	bool set_contains(struct set *s,symb i);
	
	#define SET_INCLUDED
#endif