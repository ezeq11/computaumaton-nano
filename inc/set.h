#ifndef SET_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	#include "queue_read.h"
	#include "bit_array.h"
	
	BIT_ARRAY_DECL(SYMBOL_COUNT)
	extern const struct queue_read_io_config SET_READ_CONFIG;
	
	struct element;
	struct product;
	
	struct set{
		struct set     *superset;
		
		struct set     *subset;
		struct element *element;
		struct product *product;
		
		struct queue_read read;
		bit_array(SYMBOL_COUNT) members;
	};
	
	#define SET_INIT(SUPERSET,SUBSET,ELEMENT,PRODUCT) {(SUPERSET),(SUBSET),(ELEMENT),(PRODUCT),QUEUE_READ_INIT((SUPERSET),NULL,&(SET_READ_CONFIG))}
	
	void set_add   (struct set *s,symb i);
	void set_remove(struct set *s,symb i);
	
	bool set_contains(struct set *s,symb i);
	
	// ------------------------------------------------------------ ||
	
	void set_update(struct set *s,int in,bool is_switching);
	void set_draw(int y,int x,struct set *s);
	
	#define SET_INCLUDED
#endif