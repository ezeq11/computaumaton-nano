#include <stddef.h>

#include "set.h"
#include "product.h"
#include "bit_array.h"

// ------------------------------------------------------------ ||

void product_q_clear(struct product *p){
	if(p == NULL){
		return;
	}
	
	p->q_element = SYMBOL_COUNT;
	p->q_element_written = 0;
	
	product_q_clear(p->subproduct);
}

void product_q_enqueue(struct product *p,symb val){
	if(p == NULL){
		return;
	}
	
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	if(p->q_element_written){
		product_q_enqueue(p->subproduct,val);
		
	}else if(p->superset == NULL || set_contains(p->superset,val)){
		p->q_element = val;
		p->q_element_written = 1;
	}
}

void product_q_dequeue(struct product *p){
	if(p == NULL){
		return;
	}
	
	if(!(p->q_element_written)){
		return;
	}
	
	if(p->subproduct == NULL || !(p->subproduct->q_element_written)){
		p->q_element = SYMBOL_COUNT;
		p->q_element_written = 0;
	}else{
		product_q_dequeue(p->subproduct);
	}
}

static bool product_q_is_complete(struct product *p){
	if(p == NULL){
		return 1;
	}
	
	return p->q_element_written && product_q_is_complete(p->subproduct);
}

// ------------------------------------------------------------ ||

symb addition_element;
bool addition_index_walk;
uint addition_index;

static void product_q_add_locate_walk(uint i,symb val){
	if(!addition_index_walk){
		return;
	}
	
	if(i < addition_index){
		return;
	}
	
	if(val < addition_element){
		addition_index = i + 1;
	}else{
		addition_index_walk = 0;
	}
}

static void product_q_add_locate(struct product *p){
	if(p == NULL){
		return;
	}
	
	addition_element = p->q_element;
	addition_index_walk = 1;
	symb_list_forall(&(p->list),&product_q_add_locate_walk);
	
	product_q_add_locate(p->subproduct);
}

static void product_q_add_insert(struct product *p,uint i){
	if(p == NULL){
		return;
	}
	
	symb_list_insert(&(p->list),i,p->q_element);
	
	product_q_add_insert(p->subproduct,i);
}

bool product_q_add(struct product *p){
	if(!product_q_is_complete(p)){
		return 0;
	}
	
	addition_index = 0;
	product_q_add_locate(p);
	
	product_q_add_insert(p,addition_index);
	
	return 1;
}

// ------------------------------------------------------------ ||

symb val_to_mark;

BIT_ARRAY_DECL(SYMB_LIST_BLOCK_LEN)
bit_array(SYMB_LIST_BLOCK_LEN) marked_elements;

static void product_marked_clear(){
	bit_array_clear(marked_elements,SYMB_LIST_BLOCK_LEN);
}

static void product_mark_eq_walk(uint i,symb val){
	if(val == val_to_mark){
		bit_array_add(marked_elements,i);
	}
}

static void product_mark_neq_walk(uint i,symb val){
	if(val != val_to_mark){
		bit_array_add(marked_elements,i);
	}
}

static bool product_is_marked_walk(uint i,symb val){
	return bit_array_get(marked_elements,i);
}

static bool product_isnt_marked_walk(uint i,symb val){
	return !bit_array_get(marked_elements,i);
}

// ------------------------------------------------------------ ||

static void product_q_remove_mark(struct product *p){
	if(p == NULL){
		return;
	}
	
	val_to_mark = p->q_element;
	symb_list_forall(&(p->list),&product_mark_neq_walk);
	
	product_q_remove_mark(p->subproduct);
}

static void product_q_remove_erase(struct product *p){
	if(p == NULL){
		return;
	}
	
	symb_list_removeif(&(p->list),&product_isnt_marked_walk);
	
	product_q_remove_erase(p->subproduct);
}

bool product_q_remove(struct product *p){
	if(!product_q_is_complete(p)){
		return 0;
	}
	
	product_marked_clear();
	product_q_remove_mark(p);
	
	product_q_remove_erase(p);
	
	return 1;
}

// ------------------------------------------------------------ ||

static void product_remove_referencing_mark(struct product *p,struct set *s,symb val){
	if(p == NULL){
		return;
	}
	
	if(p->superset == s){
		val_to_mark = val;
		symb_list_forall(&(p->list),&product_mark_eq_walk);
	}
	
	product_remove_referencing_mark(p->subproduct,s,val);
}

static void product_remove_referencing_erase(struct product *p){
	if(p == NULL){
		return;
	}
	
	symb_list_removeif(&(p->list),&product_is_marked_walk);
	
	product_remove_referencing_erase(p->subproduct);
}

void product_remove_referencing(struct product *p,struct set *s,symb val){
	product_marked_clear();
	product_remove_referencing_mark(p,s,val);
	
	product_remove_referencing_erase(p);
}

// ------------------------------------------------------------ ||

uint product_size(struct product *p){
	if(p == NULL){
		return 0;
	}
	
	uint this_size = symb_list_len(&(p->list));
	uint sub_size = product_size(p->subproduct);
	
	return this_size > sub_size ? this_size : sub_size;
}

void product_forqueue(struct product *p,void (*f)(symb,bool)){
	if(p == NULL){
		return;
	}
	
	f(p->q_element,p->subproduct == NULL);
	
	product_forqueue(p->subproduct,f);
}

void product_fortuple(struct product *p,uint i,void (*f)(symb,bool)){
	if(p == NULL){
		return;
	}
	
	f(symb_list_get(&(p->list),i),p->subproduct == NULL);
	
	product_fortuple(p->subproduct,i,f);
}