#include "set.h"

const struct chain_type SET_TYPE = {
	// Callbacks
	.size = &set_size,
	symb (* const get)      (const void *,uint);
	bool (* const contains) (const void *,symb);
	
	void (* const on_add_init)        ();
	void (* const on_add)             (void *,symb);
	void (* const on_add_complete)    (void *);
	
	void (* const on_remove_init)     ();
	void (* const on_and_remove)      (void *,symb);
	void (* const on_or_remove)       (void *,symb);
	void (* const on_remove_complete) (void *);
	
	void (* const on_set_init)        ();
	void (* const on_set)             (void *,symb);
	void (* const on_set_complete)    (void *);
	
	void (* const on_clear_init)      ();
	void (* const on_clear)           (void *);
	void (* const on_clear_complete)  (void *);
	
	// Drawing parameters
	const bool paranthesize;
	const bool bracket;
	
	const bool wrap_sideways;
	const uint wrap_size;
};

uint set_size(const void *s){
	return bit_array_size((const set *)s,SYMBOL_COUNT);
}

symb set_get(const void *,uint i){
	
}

void set_add(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_add(s->members,(uint)i);
}

void set_remove(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_remove(s->members,(uint)i);
}

bool set_contains(const struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return 0;
	}
	
	return bit_array_get(s->members,(uint)i);
}