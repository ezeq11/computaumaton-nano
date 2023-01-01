#include <string.h>

#include "bit_array.h"

void ba_clear(uint8_t *array,uint bit_count){
	memset(array,0,BITS_BYTE_LEN(bit_count) * sizeof(uint8_t));
}

uint ba_get(uint8_t *array,uint i){
	return (array[i / 8] >> (i % 8)) & 0x1;
}

void ba_add(uint8_t *array,uint i){
	array[i / 8] |=  (0x1 << (i % 8));
}

void ba_remove(uint8_t *array,uint i){
	array[i / 8] &= ~(0x1 << (i % 8));
}

void ba_add_masked(uint8_t *array,uint i,uint8_t *mask){
	array[i / 8] |= (0x1 << (i % 8)) & mask[i / 8];
}