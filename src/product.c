#include "product.h"
#include "bit_array.h"

const struct chain_type PRODUCT_TYPE = {
	// Update
	.contains = &product_contains,
	
	.on_add_init = &product_add_init,
	.on_add = &product_add,
	.on_add_complete = &product_add_complete,
	
	.on_and_remove_init = &product_remove_init,
	.on_and_remove = &product_and_remove,
	.on_and_remove_complete = &product_and_remove_complete,
	
	.on_or_remove_init = &product_remove_init,
	.on_or_remove = &product_or_remove,
	.on_or_remove_complete = &product_or_remove_complete,
	
	// Draw
	.paranthesize = true,
	.bracket = true,
	
	.wrap_size = 8,
	
	.size = &product_size,
	
	.draw_vertical_get = &product_get
};

// ------------------------------------------------------------ ||

BIT_ARRAY_DECL(SYMB_LIST_BLOCK_LEN)

static symb product_val_to_mark;
static bit_array(SYMB_LIST_BLOCK_LEN) product_marked_elements;

static void product_marked_clear(){
	bit_array_clear(product_marked_elements,SYMB_LIST_BLOCK_LEN);
}

static void product_mark_eq_walk(uint i,symb val){
	if(val == product_val_to_mark){
		bit_array_add(product_marked_elements,i);
	}
}

static void product_mark_neq_walk(uint i,symb val){
	if(val != product_val_to_mark){
		bit_array_add(product_marked_elements,i);
	}
}

static bool product_is_marked_walk(uint i,symb val){
	return bit_array_get(product_marked_elements,i);
}

static bool product_isnt_marked_walk(uint i,symb val){
	return !bit_array_get(product_marked_elements,i);
}

// ------------------------------------------------------------ ||

uint product_size(const void *p){
	return symb_list_size((const product *)p);
}

bool product_contains(const void *p,symb val){
	return symb_list_contains((const product *)p,val);
}

// ------------------------------------------------------------ ||

static uint product_add_index;
static symb product_add_val;
static bool product_add_index_walk;

static void product_add_locate(uint i,symb val){
	if(!product_add_index_walk){
		return;
	}
	
	if(i < product_add_index){
		return;
	}
	
	if(val < product_add_val){
		product_add_index = i + 1;
	}else{
		product_add_index_walk = 0;
	}
}

void product_add_init(){
	// Locate
	product_add_index = 0;
	
	// Uniqueness
	product_marked_clear();
}

void product_add(void *p,symb val){
	// Locate
	product_add_val = val;
	product_add_index_walk = 1;
	
	symb_list_forall((product *)p,&product_add_locate);
	
	// Uniqueness
	product_val_to_mark = val;
	symb_list_forall((product *)p,&product_mark_neq_walk);
}

void product_add_complete(void *p,symb val){
	// Verify uniqueness
	uint size = symb_list_len((const product *)p);
	
	if(bit_array_size(product_marked_elements,size) != size){
		return;
	}
	
	// Insert
	symb_list_insert((product *)p,product_add_index,val);
}

// ------------------------------------------------------------ ||

void product_remove_init(){
	product_marked_clear();
}

void product_and_remove(void *p,symb val){
	product_val_to_mark = val;
	
	symb_list_forall((product *)p,&product_mark_neq_walk);
}

void product_and_remove_complete(void *p,symb val){
	symb_list_removeif((product *)p,&product_isnt_marked_walk);
}

void product_or_remove(void *p,symb val){
	product_val_to_mark = val;
	
	symb_list_forall((product *)p,&product_mark_eq_walk);
}

void product_or_remove_complete(void *p,symb val){
	symb_list_removeif((product *)p,&product_is_marked_walk);
}

// ------------------------------------------------------------ ||

symb product_get(const void *p,uint i){
	return symb_list_get((const product *)p,i);
}