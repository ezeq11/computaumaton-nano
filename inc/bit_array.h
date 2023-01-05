#ifndef BIT_ARRAY_INCLUDED
	#include <stdint.h>
	
	#include "unsigned.h"
	
	#define BITS_BYTE_LEN(BIT_LEN) (((BIT_LEN) / 8) + ((BIT_LEN) % 8 > 0))
	
	#define bit_array_(BIT_LEN) bit_array_ ## BIT_LEN
	#define bit_array(BIT_LEN) bit_array_(BIT_LEN)
	
	#define BIT_ARRAY_DECL(BIT_LEN) typedef uint8_t bit_array(BIT_LEN)[BITS_BYTE_LEN(BIT_LEN)];
	
	void bit_array_clear     (uint8_t *array,uint bit_count);
	void bit_array_add       (uint8_t *array,uint i);
	void bit_array_add_masked(uint8_t *array,uint i,const uint8_t *mask_array);
	void bit_array_remove    (uint8_t *array,uint i);
	
	bool bit_array_get(const uint8_t *array,uint i);
	
	uint bit_array_size    (const uint8_t *array,uint bit_count);
	bool bit_array_is_empty(const uint8_t *array,uint bit_count);
	void bit_array_forall  (const uint8_t *array,uint bit_count,void (*f)(uint));
	
	#define BIT_ARRAY_INCLUDED
#endif