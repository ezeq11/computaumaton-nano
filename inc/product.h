#ifndef PRODUCT_INCLUDED
	#include "unsigned.h"
	
	#include "vlen_array.h"
	
	struct set;
	
	struct product{
		struct set     *superset;
		struct product *subproduct;
		
		uchar new_element_written;
		uchar new_element;
		
		struct vlen_array list;
	};
	
	#define PRODUCT_INIT(SUPERSET_INIT,SUBPRODUCT_INIT) {SUPERSET_INIT,SUBPRODUCT_INIT,0,0,VLEN_ARRAY_INIT}
	
	uint product_queue_new  (struct product *p,uchar val);
	void product_queue_clear(struct product *p);
	void product_add_new    (struct product *p);
	
	void product_remove(struct product *p,uint i);
	void product_remove_referencing(struct product *p,struct set *s,uchar val);
	
	#define PRODUCT_INCLUDED
#endif