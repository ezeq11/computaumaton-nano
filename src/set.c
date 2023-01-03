#include <stddef.h>

#include "set.h"
#include "product.h"

void set_q_clear(struct set *s){
	s->q_element = SYMBOL_COUNT;
	s->q_element_written = 0;
}

void set_q_enqueue(struct set *s,symb val){
	if(s->q_element_written){
		return;
	}
	
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	if(s->superset == NULL || set_contains(s->superset,val)){
		s->q_element = val;
		s->q_element_written = 1;
	}
}

void set_q_dequeue(struct set *s){
	s->q_element = SYMBOL_COUNT;
	s->q_element_written = 0;
}

symb set_q_get(struct set *s){
	return s->q_element;
}

bool set_q_add(struct set *s){
	if(!(s->q_element_written)){
		return 0;
	}
	
	set_add(s,s->q_element);
	
	return 1;
}

bool set_q_remove(struct set *s){
	if(!(s->q_element_written)){
		return 0;
	}
	
	set_remove(s,s->q_element);
	
	return 1;
}

void set_add(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	if(s->superset != NULL){
		bit_array_add_masked(s->members,(uint)i,s->superset->members);
	}else{
		bit_array_add(s->members,(uint)i);
	}
}

void set_remove(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_remove(s->members,(uint)i);
	
	if(s->product != NULL){
		product_remove_referencing(s->product,s,i);
	}
	
	if(s->subset != NULL){
		set_remove(s->subset,i);
	}
}

bool set_contains(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return 0;
	}
	
	return bit_array_get(s->members,(uint)i);
}