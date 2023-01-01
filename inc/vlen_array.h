#ifndef VLEN_ARRAY_INCLUDED
	#include "unsigned.h"
	
	#define VLEN_ARRAY_BLOCK_LEN 128
	
	struct vlen_array{
		uint  len;
		uchar block[VLEN_ARRAY_BLOCK_LEN];
	};
	
	#define VLEN_ARRAY_INIT {0}
	
	uint vlen_array_len(struct vlen_array *array);
	
	void vlen_array_insert(struct vlen_array *array,uint i,uchar val);
	void vlen_array_remove(struct vlen_array *array,uint i);
	
	uint vlen_array_get(struct vlen_array *array,uint i);
	void vlen_array_set(struct vlen_array *array,uint i,uchar val);
	
	void vlen_array_forall  (struct vlen_array *array,void (*f)(uint,uchar));
	void vlen_array_removeif(struct vlen_array *array,uint (*f)(uint,uchar));
	uint vlen_array_contains(struct vlen_array *array,uchar val);
	
	#define VLEN_ARRAY_INCLUDED
#endif