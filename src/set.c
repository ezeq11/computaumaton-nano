#include "set.h"

const struct chain_type SET_TYPE = {
	// Update
	.contains = &set_contains,
	
	.on_add = &set_add,
	
	.on_and_remove = &set_remove,
	.on_or_remove = &set_remove,
	
	// Draw
	.paranthesize = false,
	.bracket = true,
	
	.wrap_size = 20,
	
	.size = &set_size,
	
	.draw_horizontal_iter_begin = &set_iter_begin,
	.draw_horizontal_iter_seek = &set_iter_seek,
	.draw_horizontal_iter_get = &set_iter_get
};

// ------------------------------------------------------------ ||

uint set_size(const void *s){
	return bit_array_size(*((const set *)s),SYMBOL_COUNT);
}

bool set_contains(const void *s,symb val){
	if(val >= SYMBOL_COUNT){
		return 0;
	}
	
	return bit_array_get(*((const set *)s),(uint)val);
}

void set_add(void *s,symb val){
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_add(*((set *)s),(uint)val);
}

void set_remove(void *s,symb val){
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_remove(*((set *)s),(uint)val);
}

// ------------------------------------------------------------ ||

static bit_array_iter set_iter;

bool set_iter_begin(const void *s){
	return bit_array_iter_begin(*((const set *)s),SYMBOL_COUNT,&set_iter);
}

bool set_iter_seek(const void *s){
	return bit_array_iter_seek(*((const set *)s),SYMBOL_COUNT,&set_iter);
}

symb set_iter_get(const void *s){
	return (symb)bit_array_iter_get(*((const set *)s),SYMBOL_COUNT,&set_iter);
}