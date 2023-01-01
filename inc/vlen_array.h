#ifndef VLEN_ARRAY_INCLUDED
	#include "unsigned.h"
	
	#define VLEN_ARRAY_BLOCK_LEN 128
	
	struct vlen_array{
		uint len;
		uchar block[VLEN_ARRAY_BLOCK_LEN];
	};
	
	#define VLEN_ARRAY_INIT {0}
	
	uint vla_len(struct vlen_array *array);
	
	void vla_insert(struct vlen_array *array,uint i,uchar val);
	void vla_remove(struct vlen_array *array,uint i);
	
	uint vla_get(struct vlen_array *array,uint i);
	void vla_set(struct vlen_array *array,uint i,uchar val);
	
	void vla_forall  (struct vlen_array *array,void (*f)(uint,uchar));
	void vla_removeif(struct vlen_array *array,uint (*f)(uint,uchar));
	uint vla_contains(struct vlen_array *array,uchar val);
	
	#define VLEN_ARRAY_INCLUDED
#endif