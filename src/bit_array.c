#include <string.h>

#include "bit_array.h"

void bit_array_clear(uint8_t *array,uint bit_count){
	memset(array,0,BITS_BYTE_LEN(bit_count) * sizeof(uint8_t));
}

void bit_array_add(uint8_t *array,uint i){
	array[i / 8] |=  (0x1 << (i % 8));
}

void bit_array_add_masked(uint8_t *array,uint i,const uint8_t *mask_array){
	array[i / 8] |= (0x1 << (i % 8)) & mask_array[i / 8];
}

void bit_array_remove(uint8_t *array,uint i){
	array[i / 8] &= ~(0x1 << (i % 8));
}

bool bit_array_get(const uint8_t *array,uint i){
	return (array[i / 8] >> (i % 8)) & 0x1;
}

static void bit_array_forbytes(const uint8_t *array,uint bit_count,void (*f)(uint8_t)){
	uint bytes = BITS_BYTE_LEN(bit_count);
	bool partial_last_byte = (bit_count % 8 > 0);
	
	for(uint byte = 0;byte < bytes;++byte){
		if(partial_last_byte && byte + 1 == bytes){
			uint last_byte = array[byte] & (uint8_t)~(0xff << (bit_count % 8));
			
			f(last_byte);
			
		}else{
			f(array[byte]);
		}
	}
}

const uint nibble_size[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

static uint size;
static void bit_array_size_add(uint8_t byte){
	size += nibble_size[(byte >> 0) & 0xf];
	size += nibble_size[(byte >> 4) & 0xf];
}

uint bit_array_size(const uint8_t *array,uint bit_count){
	size = 0;
	bit_array_forbytes(array,bit_count,&bit_array_size_add);
	
	return size;
}

static bool empty;
static void bit_array_is_empty_conjugate(uint8_t byte){
	empty = empty && (byte == 0);
}

bool bit_array_is_empty(const uint8_t *array,uint bit_count){
	empty = 1;
	bit_array_forbytes(array,bit_count,&bit_array_is_empty_conjugate);
	
	return empty;
}

void bit_array_forall(const uint8_t *array,uint bit_count,void (*f)(uint)){
	// First pass: byte-level scan (8 bits at a time)
	for(uint byte = 0;byte < BITS_BYTE_LEN(bit_count);++byte){
		if(array[byte] == 0){
			continue;
		}
		
		// Second pass: bit-level scan
		for(uint bit = 0;bit < 8;++bit){
			uint i = byte * 8 + bit;
			
			if(i >= bit_count){
				continue;
			}
			
			if(!bit_array_get(array,i)){
				continue;
			}
			
			f(i);
		}
	}
}

// ------------------------------------------------------------ ||

bool bit_array_iter_begin(const uint8_t *array,uint bit_count,bit_array_iter *iter){
	(*iter) = 0;
	
	if(bit_count == 0){
		return 0;
	}else if(bit_array_get(array,*iter)){
		return 1;
	}else{
		return bit_array_iter_seek(array,bit_count,iter);
	}
}

bool bit_array_iter_seek(const uint8_t *array,uint bit_count,bit_array_iter *iter){
	++(*iter);
	
	while((*iter) < bit_count && !bit_array_get(array,*iter)){
		if(array[(*iter) / 8] == 0){
			(*iter) = (((*iter) / 8) + 1) * 8;
		}else{
			++(*iter);
		}
	}
	
	return (*iter) < bit_count;
}

uint bit_array_iter_get(const uint8_t *array,uint bit_count,const bit_array_iter *iter){
	if((*iter) > bit_count){
		return bit_count;
	}
	
	if(bit_array_get(array,*iter)){
		return *iter;
	}else{
		return bit_count;
	}
}