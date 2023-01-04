#ifndef ELEMENT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	struct set;
	
	struct element{
		struct set *superset;
		
		symb q_val;
		symb   val;
	};
	
	#define ELEMENT_INIT(SUPERSET) {SUPERSET,SYMBOL_COUNT,SYMBOL_COUNT}
	
	void element_q_clear(struct element *e);
	void element_q_enqueue(struct element *e,symb val);
	void element_q_dequeue(struct element *e);
	symb element_q_get(struct element *e);
	
	bool element_q_add(struct element *e);
	
	symb element_get(struct element *e);
	
	#define ELEMENT_INCLUDED
#endif