#include <stddef.h>

#include "element.h"
#include "set.h"

const struct queue_read_io_config ELEMENT_READ_CONFIG = QUEUE_READ_IO_CONFIG_INIT(0,'=','/',PREFIX_DISABLED,PREFIX_DISABLED,0,0);

void element_set(struct element *e,symb val){
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	if(e->superset == NULL || set_contains(e->superset,val)){
		e->value = val;
	}
}

void element_unset(struct element *e,symb val){
	if(e->value == val){
		e->value = SYMBOL_COUNT;
	}
}

void element_unset_referencing(struct element *e,struct set *s,symb val){
	if(e->superset == s){
		element_unset(e,val);
	}
}

symb element_get(const struct element *e){
	return e->value;
}

// ------------------------------------------------------------ ||

struct element *element_current;

static void element_read_on_submit(enum queue_read_mode mode){
	switch(mode){
	case QUEUE_READ_IDEMPOTENT:
		// null
		
		break;
	case QUEUE_READ_ADD:
		element_set(element_current,queue_read_value(&(element_current->read)));
		
		break;
	case QUEUE_READ_REMOVE:
		element_unset(element_current,queue_read_value(&(element_current->read)));
		
		break;
	}
}

void element_update(struct element *e,int in,bool is_switching){
	element_current = e;
	queue_read_update(&(e->read),in,is_switching,&element_read_on_submit);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

void element_draw(int y,int x,const struct element *e){
	move(y,x);
	
	if(queue_read_mode(&(e->read)) != QUEUE_READ_IDEMPOTENT){
		queue_read_draw(&(e->read));
	}else{
		addch(ascii(e->value));
	}
}