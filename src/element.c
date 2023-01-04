#include <stddef.h>

#include "element.h"
#include "set.h"

void element_q_clear(struct element *e){
	e->q_val = SYMBOL_COUNT;
}

void element_q_enqueue(struct element *e,symb val){
	if(e->q_val != SYMBOL_COUNT){
		return;
	}
	
	if(e->superset == NULL || set_contains(e->superset,val)){
		e->q_val = val;
	}
}

void element_q_dequeue(struct element *e){
	e->q_val = SYMBOL_COUNT;
}

symb element_q_get(struct element *e){
	return e->q_val;
}

bool element_q_add(struct element *e){
	if(e->q_val == SYMBOL_COUNT){
		return 0;
	}
	
	e->val = e->q_val;
	return 1;
}

symb element_get(struct element *e){
	return e->val;
}