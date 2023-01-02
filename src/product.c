#include "product.h"
#include "bit_array.h"

uint product_queue_new(struct product *p,uchar val){
	if(p->new_element_written){
		if(p->subproduct == NULL){
			return 1;
		}else{
			return product_queue_new(p->subproduct,val);
		}
	}else{
		p->new_element = val;
		p->new_element_written = 1;
		
		return 0;
	}
}

void product_queue_clear(struct product *p){
	if(p == NULL){
		return;
	}
	
	p->new_element_written = 0;
	
	product_queue_clear(p->subproduct);
}

uchar addition_element;
uchar addition_index_walk;
uint  addition_index;

static void product_add_locate_walk(uint i,uchar val){
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

static void product_add_locate(struct product *p){
	if(p == NULL){
		return;
	}
	
	addition_element = p->new_element;
	addition_index_walk = 1;
	vlen_array_forall(&(p->list),&product_add_locate_walk);
	
	product_add_locate(p->subproduct);
}

static void product_add_insert(struct product *p,uint i){
	if(p == NULL){
		return;
	}
	
	vlen_array_insert(&(p->list),i,p->new_element);
	p->new_element_written = 0;
	
	product_add_insert(p->subproduct,i);
}

void product_add_new(struct product *p){
	addition_index = 0;
	product_add_locate(p);
	
	product_add_insert(p,addition_index);
}

// ------------------------------------------------------------ ||

void product_remove(struct product *p,uint i){
	if(p == NULL){
		return;
	}
	
	vlen_array_remove(&(p->list),i);
	
	product_remove(p->subproduct,i);
}

// ------------------------------------------------------------ ||

uchar val_to_remove;

BIT_ARRAY_DECL(VLEN_ARRAY_BLOCK_LEN)
bit_array(VLEN_ARRAY_BLOCK_LEN) elements_to_remove;

static void product_remove_mark_walk(uint i,uchar val){
	if(val == val_to_remove){
		bit_array_add(elements_to_remove,i);
	}
}

static void product_remove_mark(struct product *p,struct set *s){
	if(p == NULL){
		return;
	}
	
	if(p->superset == s){
		vlen_array_forall(&(p->list),&product_remove_mark_walk);
	}
	
	product_remove_mark(p->subproduct,s);
}

static uint product_remove_erase_walk(uint i,uchar val){
	return bit_array_get(elements_to_remove,i);
}

static void product_remove_erase(struct product *p){
	if(p == NULL){
		return;
	}
	
	vlen_array_removeif(&(p->list),&product_remove_erase_walk);
	
	product_remove_erase(p->subproduct);
}

void product_remove_referencing(struct product *p,struct set *s,uchar val){
	val_to_remove = val;
	bit_array_clear(elements_to_remove,VLEN_ARRAY_BLOCK_LEN);
	product_remove_mark(p,s);
	
	product_remove_erase(p);
}