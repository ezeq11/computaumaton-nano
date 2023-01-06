#ifndef PRODUCT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "queue_read.h"
	#include "symb_list.h"
	
	extern const struct queue_read_io_config PRODUCT_READ_CONFIG;
	
	struct set;
	
	struct product{
		const struct set * const superset;
		struct product   * const subproduct;
		
		struct queue_read read;
		struct symb_list list;
	};
	
	#define PRODUCT_INIT_LINK(SUPERSET,SUBPRODUCT) {\
		(SUPERSET),\
		(SUBPRODUCT),\
		\
		QUEUE_READ_INIT((SUPERSET),&((SUBPRODUCT)->read),&PRODUCT_READ_CONFIG),\
		SYMB_LIST_INIT\
	}
	
	#define PRODUCT_INIT_TAIL(SUPERSET) {\
		(SUPERSET),\
		NULL,\
		\
		QUEUE_READ_INIT((SUPERSET),NULL,&PRODUCT_READ_CONFIG),\
		SYMB_LIST_INIT\
	}
	
	void product_remove_referencing(struct product *p,struct set *s,symb val);
	
	// ------------------------------------------------------------ ||
	
	void product_update(struct product *p,int in,bool is_switching);
	int  product_draw(int y,int x,const struct product *p,uint max_rows);
	
	#define PRODUCT_INCLUDED
#endif