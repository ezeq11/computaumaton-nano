#ifndef LINK_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	struct link_funcset{
		uint (* const size)     (const void *);
		symb (* const get)      (const void *,uint);
		bool (* const contains) (const void *,symb);
		
		void (* const on_add_init)        ();
		void (* const on_add)             (void *,symb);
		void (* const on_add_complete)    (void *);
		
		void (* const on_remove_init)     ();
		void (* const on_and_remove)      (void *,symb);
		void (* const on_or_remove)       (void *,symb);
		void (* const on_remove_complete) (void *);
		
		void (* const on_set_init)        ();
		void (* const on_set)             (void *,symb);
		void (* const on_set_complete)    (void *);
		
		void (* const on_clear_init)      ();
		void (* const on_clear)           (void *);
		void (* const on_clear_complete)  (void *);
	};
	
	struct link_head;
	
	struct link{
		// Link structure
		struct link_head * const head;
		struct link * const next;
		
		// Local callback parameters, read data
		void * const object;
		symb read_val;
	};
	
	#define LINK_INIT(HEAD,OBJECT,NEXT) {(HEAD),(NEXT),(OBJECT),SYMBOL_COUNT}
	
	struct link_relation{
		struct link * const super;
		struct link * const sub;
	};
	
	#define LINK_RELATION_INIT(SUPER,SUB) {(SUPER),(SUB)}
	
	enum link_read{
		LINK_IDEMPOTENT,
		
		LINK_ADD,
		LINK_REMOVE,
		
		LINK_SET
	};
	
	struct link_head{
		// Link structure
		struct link * const next;
		
		// Global callbacks, read state
		const struct link_funcset * const funcset;
		enum link_read read;
		
		// Drawing parameters
		const uint transition_pos;
		const uint nonvariadic_len;
		
		const bool paranthesize;
		const bool bracket;
		
		const bool wrap_sideways;
		const uint wrap_size;
	};
	
	#define LINK_HEAD_INIT(FUNCSET,NEXT) {(NEXT),(FUNCSET),LINK_IDEMPOTENT}
	
	void chain_update(struct link_head *head,int in,struct link_relation *relations,uint relations_size);
	int chain_draw(const struct link_head *head,int y,int x);
	
	#define LINK_INCLUDED
#endif