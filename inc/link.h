#ifndef LINK_INCLUDED
	#include "symbol.h"
	
	enum link_read{
		LINK_IDEMPOTENT,
		
		LINK_ADD,
		LINK_REMOVE,
		
		LINK_SET
	};
	
	struct set;
	
	struct link{
		// Link structure
		const struct set  * const superset;
		struct link * const next;
		
		// Local callback parameters
		void * const object;
		
		// Local read data
		symb read_val;
	};
	
	#define LINK_INIT(SUPERSET,NEXT,OBJECT) {(SUPERSET),(NEXT),(OBJECT),SYMBOL_COUNT}
	
	struct link_head{
		// Link structure
		struct link * const next;
		
		// Global callbacks
		void (* const on_add)    (void *,symb);
		void (* const on_remove) (void *,symb);
		
		void (* const on_set)    (void *,symb);
		void (* const on_clear)  (void *);
		
		// Global read state
		enum link_read read;
	};
	
	#define LINK_HEAD_INIT(NEXT,ON_ADD,ON_REMOVE,ON_SET,ON_CLEAR) {(NEXT),(ON_ADD),(ON_REMOVE),(ON_SET),(ON_CLEAR),LINK_IDEMPOTENT}
	
	void links_update(struct link_head *head,int in);
	
	#define LINK_INCLUDED
#endif