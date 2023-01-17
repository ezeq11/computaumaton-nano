#ifndef SYMB_LIST_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#define SYMB_LIST_BLOCK_LEN 128
	
	struct symb_list{
		uint len;
		symb block[SYMB_LIST_BLOCK_LEN];
	};
	
	#define SYMB_LIST_INIT { .len = 0 }
	
	uint symb_list_len(const struct symb_list *list);
	void symb_list_clear(struct symb_list *list);
	
	// Array-list operation
	void symb_list_insert(struct symb_list *list,uint i,symb val);
	void symb_list_remove(struct symb_list *list,uint i);
	
	symb symb_list_get(const struct symb_list *list,uint i);
	void symb_list_set(      struct symb_list *list,uint i,symb val);
	
	// Stack operation
	void symb_list_push(struct symb_list *list,symb val);
	void symb_list_pop (struct symb_list *list);
	
	symb symb_list_top (const struct symb_list *list);
	
	// Iteratives
	void symb_list_forall  (const struct symb_list *list,void (*f)(uint,symb));
	void symb_list_removeif(      struct symb_list *list,bool (*f)(uint,symb));
	bool symb_list_contains(const struct symb_list *list,symb val);
	
	#define SYMB_LIST_INCLUDED
#endif