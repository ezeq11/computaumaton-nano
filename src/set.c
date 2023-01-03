#include <stddef.h>

#include "set.h"
#include "product.h"

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