#ifndef LINK_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	struct chain_type{
		// Update callbacks
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
		
		// Draw configuration
		const bool paranthesize;
		const bool bracket;
		
		const uint wrap_size;
		
		/*
		  If draw_horizontal_ functions are non-NULL, they will be used to draw objects horizontally.
		  Otherwise, draw_vertical_ functions must be non-NULL, and they will be used to draw objects
		  vertically.
		  
		  Each set of functions represents a different mode of iteration - draw_horizontal_ is sequential,
		  while draw_vertical_ is random-access. This allows the interface implementer to choose whichever
		  mode is most performant for their use case.
		  
		  It should be noted that these cases (horizontal vs vertical draw, sequential vs random-access retrieval)
		  are inseparable. In the interest of reducing calls to move(), it must be that iteration must occur
		  per on-screen line.
		  
		  To illustrate, take the example E = {0,1,2,3,4,5,6,7,8,9} with wrap_size = 2.
		  
		  Horizontally, it is:
		    0 1
		    2 3
		    4 5
		    6 7
		    8 9
		  
		  While vertically, it is:
		    0 2 4 6 8
		    1 3 5 7 9
		  
		  Assume a function addNext() exists, which retrieves the next iteration element of E and prints it to the
		  screen via addch().
		  Using sequential iteration, we *could* generate the vertical representation of E with:
		    move(1,1);
		    addNext(); // 0
		    move(2,1);
		    addNext(); // 1
		    
		    move(1,2);
		    addNext(); // 2
		    move(2,2);
		    addNext(); // 3
		    
		    move(1,3);
		    addNext(); // 4
		    move(2,3);
		    addNext(); // 5
		    
		    move(1,4);
		    addNext(); // 6
		    move(2,4);
		    addNext(); // 7
		    
		    move(1,5);
		    addNext(); // 8
		    move(2,5);
		    addNext(); // 9
		  
		  Now assume a function addGet(i) exists, which retrieves the i-th element of E and prints it to the
		  screen via addch();
		  Then we could simply do:
		    move(1,1);
		    addGet(0);
		    addGet(2);
		    addGet(4);
		    addGet(6);
		    addGet(8);
		    
		    move(2,1);
		    addGet(1);
		    addGet(3);
		    addGet(5);
		    addGet(7);
		    addGet(9);
		  
		  Where move() would have once been called len(E) times, it is now only called wrap_size times. Phew!
		  Thus, random access is nice for vertical drawing :)
		  
		  For all other purposes (ie, horizontal drawing), sequential iteration is just fine.
		  It's easy to implement sequential iteration on a random access structure; just do ++iter for
		  some indexing uint iter. That's O(1).
		  It's more difficult to implement random-access retrieval on a sequential structure. Each iteration
		  until the i-th element is O(n); over n elements, that becomes O(n^2).
		*/
		
		uint (* const size) (const void *);
		
		bool (* const draw_horizontal_iter_begin) (const void *); // resets iteration to the beginning, returns 0 if end is reached, 1 otherwise
		bool (* const draw_horizontal_iter_seek)  (const void *); // finds the next value, returns 0 if end is reached, 1 otherwise
		symb (* const draw_horizontal_iter_get)   (const void *); // returns value at current position
		
		symb (* const draw_vertical_get)  (const void *,uint);
	};
	
	struct link_head;
	
	struct link{
		// Link structure
		struct link_head * const head;
		struct link * const next;
		
		// Local callback parameter, read data
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
		const struct chain_type * const type;
		struct link * const next;
		
		// Indexed parameters
		const uint nonvariadic_len;
		const uint transition_pos;
		
		// Read state
		enum link_read read;
	};
	
	#define LINK_HEAD_INIT(TYPE,NONVAR_LEN,TRANS_POS,NEXT) {(TYPE),(NEXT),(NONVAR_LEN),(TRANS_POS),LINK_IDEMPOTENT}
	
	void chain_update(struct link_head *head,int in,struct link_relation *relations,uint relations_size);
	int chain_draw(const struct link_head *head,int y,int x);
	
	#define LINK_INCLUDED
#endif