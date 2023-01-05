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

uint nibble_size[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};

uint bit_array_size(const uint8_t *array,uint bit_count){
	uint size = 0;
	
	for(uint byte = 0;byte < BITS_BYTE_LEN(bit_count);++byte){
		size += nibble_size[(array[byte] >> 0) & 0xf];
		size += nibble_size[(array[byte] >> 4) & 0xf];
	}
	
	return size;
}

bool bit_array_is_empty(const uint8_t *array,uint bit_count){
	bool empty = 1;
	
	for(uint byte = 0;byte < BITS_BYTE_LEN(bit_count);++byte){
		empty = empty && (array[byte] == 0);
	}
	
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