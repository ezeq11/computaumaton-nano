#include <stddef.h>

#include "set.h"
#include "element.h"
#include "product.h"

const struct queue_read_io_config SET_READ_CONFIG = QUEUE_READ_IO_CONFIG_INIT(1,'u','\\','U','\\',1,0);

void set_add(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	if(s->superset != NULL){
		bit_array_add_masked(s->members,(uint)i,s->superset->members);
	}else{
		bit_array_add(s->members,(uint)i);
	}
}

void set_remove(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_remove(s->members,(uint)i);
	
	if(s->subset != NULL){
		set_remove(s->subset,i);
	}
	
	if(s->element != NULL){
		element_unset_referencing(s->element,s,i);
	}
	
	if(s->product != NULL){
		product_remove_referencing(s->product,s,i);
	}
}

bool set_contains(const struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return 0;
	}
	
	return bit_array_get(s->members,(uint)i);
}

// ------------------------------------------------------------ ||

struct set *set_current;

static void set_read_on_submit(enum queue_read_mode mode){
	switch(mode){
	case QUEUE_READ_IDEMPOTENT:
		// null
		
		break;
	case QUEUE_READ_ADD:
		set_add(set_current,queue_read_value(&(set_current->read)));
		
		break;
	case QUEUE_READ_REMOVE:
		set_remove(set_current,queue_read_value(&(set_current->read)));
		
		break;
	}
}

void set_update(struct set *s,int in,bool is_switching){
	set_current = s;
	queue_read_update(&(s->read),in,is_switching,&set_read_on_submit);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

uint set_i;
uint set_size;

static void set_draw_member(uint i){
	addch(ascii(i));
	++set_i;
	
	if(set_i < set_size){
		addch(',');
		addch(' ');
	}
}

void set_draw(int y,int x,const struct set *s){
	set_i = 0;
	set_size = bit_array_size(s->members,SYMBOL_COUNT);
	
	move(y,x);
	
	addch('{');
	addch(' ');
	bit_array_forall(s->members,SYMBOL_COUNT,&set_draw_member);
	addch(' ');
	addch('}');
	
	queue_read_draw(&(s->read));
}