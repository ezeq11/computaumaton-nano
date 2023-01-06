#ifndef ELEMENT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "queue_read.h"
	
	extern const struct queue_read_io_config ELEMENT_READ_CONFIG;
	
	struct set;
	
	struct element{
		const struct set * const superset;
		
		struct queue_read read;
		symb   value;
	};
	
	#define ELEMENT_INIT(SUPERSET) {\
		(SUPERSET),\
		\
		QUEUE_READ_INIT((SUPERSET),NULL,&ELEMENT_READ_CONFIG),\
		SYMBOL_COUNT\
	}
	
	void element_set  (struct element *e,symb val);
	void element_unset(struct element *e,symb val);
	
	void element_unset_referencing(struct element *e,struct set *s,symb val);
	
	symb element_get(const struct element *e);
	
	// ------------------------------------------------------------ ||
	
	void element_update(struct element *e,int in,bool is_switching);
	int  element_draw(int y,int x,const struct element *e);
	
	#define ELEMENT_INCLUDED
#endif