#include <stddef.h>

#include "set.h"
#include "product.h"

void set_add(struct set *s,uint i){
	if(i >= CHARSET_SIZE){
		return;
	}
	
	if(s->superset != NULL){
		bit_array_add_masked(s->members,i,s->superset->members);
	}else{
		bit_array_add(s->members,i);
	}
}

void set_remove(struct set *s,uint i){
	if(i >= CHARSET_SIZE){
		return;
	}
	
	bit_array_remove(s->members,i);
	
	if(s->product != NULL){
		product_remove_referencing(s->product,s,i);
	}
	
	if(s->subset != NULL){
		set_remove(s->subset,i);
	}
}