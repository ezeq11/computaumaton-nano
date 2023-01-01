#include <string.h>

#include "vlen_array.h"

uint vlen_array_len(struct vlen_array *array){
	return array->len;
}

void vlen_array_insert(struct vlen_array *array,uint i,uchar val){
	if(array->len + 1 > VLEN_ARRAY_BLOCK_LEN){
		return;
	}
	
	if(i > array->len){
		return;
	}
	
	memmove(array->block + i + 1,array->block + i,(array->len - i) * sizeof(uchar));
	array->block[i] = val;
	
	++(array->len);
}

void vlen_array_remove(struct vlen_array *array,uint i){
	if(array->len == 0){
		return;
	}
	
	if(i >= array->len){
		return;
	}
	
	memmove(array->block + i,array->block + i + 1,(array->len - i - 1) * sizeof(uchar));
	
	--(array->len);
}

uint vlen_array_get(struct vlen_array *array,uint i){
	if(i >= array->len){
		return 0;
	}
	
	return array->block[i];
}

void vlen_array_set(struct vlen_array *array,uint i,uchar val){
	if(i >= array->len){
		return;
	}
	
	array->block[i] = val;
}

void vlen_array_forall(struct vlen_array *array,void (*f)(uint,uchar)){
	for(uint i = 0;i < array->len;++i){
		f(i,array->block[i]);
	}
}

void vlen_array_removeif(struct vlen_array *array,uint (*f)(uint,uchar)){
	uint dest = 0;
	
	for(uint src = 0;src < array->len;++src){
		if(f(src,array->block[src])){
			continue;
		}
		
		array->block[dest] = array->block[src];
		++dest;
	}
	
	array->len = dest;
}

uint vlen_array_contains(struct vlen_array *array,uchar val){
	uint found = 0;
	
	for(uint i = 0;i < array->len;++i){
		found = found || (array->block[i] == val);
	}
	
	return found;
}