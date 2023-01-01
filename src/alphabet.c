#include "alphabet.h"
#include "product.h"

void alphabet_add(struct alphabet *a,uint i){
	if(i >= CHARSET_SIZE){
		return;
	}
	
	if(a->superset != NULL){
		bit_array_add_masked(a->members,i,a->superset->members);
	}else{
		bit_array_add(a->members,i);
	}
}

void alphabet_remove(struct alphabet *a,uint i){
	if(i >= CHARSET_SIZE){
		return;
	}
	
	bit_array_remove(a->members,i);
	
	if(a->product != NULL){
		product_remove_referencing(a->product,a,i);
	}
	
	if(a->subset != NULL){
		alphabet_remove(a->subset,i);
	}
}