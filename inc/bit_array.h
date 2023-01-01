#ifndef BIT_ARRAY_INCLUDED
	#include <stdint.h>
	
	#include "unsigned.h"
	
	#define BITS_BYTE_LEN(BIT_LEN) (((BIT_LEN) / 8) + 1)
	
	#define bit_array(BIT_LEN) bit_array ## BIT_LEN
	#define BIT_ARRAY_DECL(BIT_LEN) typedef uint8_t bit_array(BIT_LEN)[BITS_BYTE_LEN(BIT_LEN)];
	
	void ba_clear(uint8_t *array,uint bit_count);
	uint ba_get(uint8_t *array,uint i);
	
	void ba_add(uint8_t *array,uint i);
	void ba_remove(uint8_t *array,uint i);
	
	void ba_add_masked(uint8_t *array,uint i,uint8_t *mask);
	
	#define BIT_ARRAY_INCLUDED
#endif