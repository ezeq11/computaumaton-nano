#include <string.h>

#include "symb_list.h"

uint symb_list_len(const struct symb_list *list){
	return list->len;
}

void symb_list_clear(struct symb_list *list){
	list->len = 0;
}

// ------------------------------------------------------------ ||

void symb_list_insert(struct symb_list *list,uint i,symb val){
	if(list->len + 1 > SYMB_LIST_BLOCK_LEN){
		return;
	}
	
	if(i > list->len){
		return;
	}
	
	memmove(list->block + i + 1,list->block + i,(list->len - i) * sizeof(symb));
	list->block[i] = val;
	
	++(list->len);
}

void symb_list_remove(struct symb_list *list,uint i){
	if(list->len == 0){
		return;
	}
	
	if(i >= list->len){
		return;
	}
	
	memmove(list->block + i,list->block + i + 1,(list->len - i - 1) * sizeof(symb));
	
	--(list->len);
}

symb symb_list_get(const struct symb_list *list,uint i){
	if(i >= list->len){
		return SYMBOL_COUNT;
	}
	
	return list->block[i];
}

void symb_list_set(struct symb_list *list,uint i,symb val){
	if(i >= list->len){
		return;
	}
	
	list->block[i] = val;
}

// ------------------------------------------------------------ ||

void symb_list_push(struct symb_list *list,symb val){
	if(list->len + 1 > SYMB_LIST_BLOCK_LEN){
		return;
	}
	
	list->block[list->len] = val;
	++(list->len);
}

void symb_list_pop(struct symb_list *list){
	if(list->len == 0){
		return;
	}
	
	(--list->len);
}

symb symb_list_top(const struct symb_list *list){
	if(list->len == 0){
		return SYMBOL_COUNT;
	}
	
	return list->block[list->len - 1];
}

// ------------------------------------------------------------ ||

void symb_list_forall(const struct symb_list *list,void (*f)(uint,symb)){
	for(uint i = 0;i < list->len;++i){
		f(i,list->block[i]);
	}
}

void symb_list_removeif(struct symb_list *list,bool (*f)(uint,symb)){
	uint dest = 0;
	
	for(uint src = 0;src < list->len;++src){
		if(f(src,list->block[src])){
			continue;
		}
		
		list->block[dest] = list->block[src];
		++dest;
	}
	
	list->len = dest;
}

bool symb_list_contains(const struct symb_list *list,symb val){
	bool found = 0;
	
	for(uint i = 0;i < list->len;++i){
		found = found || (list->block[i] == val);
	}
	
	return found;
}