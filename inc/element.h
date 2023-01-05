#ifndef ELEMENT_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "queue_read.h"
	
	extern const struct queue_read_io_config ELEMENT_READ_CONFIG;
	
	struct set;
	
	struct element{
		struct set *superset;
		
		struct queue_read read;
		symb   value;
	};
	
	#define ELEMENT_INIT(SUPERSET) {(SUPERSET),QUEUE_READ_INIT((SUPERSET),NULL,&ELEMENT_READ_CONFIG),SYMBOL_COUNT}
	
	void element_set  (struct element *e,symb val);
	void element_unset(struct element *e,symb val);
	
	void element_unset_referencing(struct element *e,struct set *s,symb val);
	
	symb element_get(struct element *e);
	
	// ------------------------------------------------------------ ||
	
	void element_update(struct element *e,int in,bool is_switching);
	void element_draw(int y,int x,struct element *e);
	
	#define ELEMENT_INCLUDED
#endif