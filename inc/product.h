#ifndef PRODUCT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "symb_list.h"
	
	struct set;
	
	struct product{
		struct set     *superset;
		struct product *subproduct;
		
		bool q_element_written;
		symb q_element;
		
		struct symb_list list;
	};
	
	#define PRODUCT_INIT(SUPERSET_INIT,SUBPRODUCT_INIT) {SUPERSET_INIT,SUBPRODUCT_INIT,0,SYMBOL_COUNT,SYMB_LIST_INIT}
	
	void product_q_clear  (struct product *p);
	void product_q_enqueue(struct product *p,symb val);
	void product_q_dequeue(struct product *p);
	
	bool product_q_add   (struct product *p);
	bool product_q_remove(struct product *p);
	
	void product_remove_referencing(struct product *p,struct set *s,symb val);
	
	uint product_size(struct product *p);
	void product_forqueue(struct product *p,void (*f)(symb,bool));
	void product_fortuple(struct product *p,uint i,void (*f)(symb,bool));
	
	#define PRODUCT_INCLUDED
#endif