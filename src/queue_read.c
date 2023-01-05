#include <stddef.h>

#include "queue_read.h"
#include "set.h"

void queue_read_init(struct queue_read *read,enum queue_read_mode mode){
	if(read == NULL){
		return;
	}
	
	read->mode = mode;
	read->value = SYMBOL_COUNT;
	
	queue_read_init(read->subqueue,mode);
}

void queue_read_enq(struct queue_read *read,symb val){
	if(read == NULL){
		return;
	}
	
	if(val >= SYMBOL_COUNT){
		return;
	}
	
	if(read->mode == QUEUE_READ_IDEMPOTENT){
		return;
	}
	
	if(read->value == SYMBOL_COUNT){
		if(read->superset == NULL || set_contains(read->superset,val)){
			read->value = val;
		}
	}else{
		queue_read_enq(read->subqueue,val);
	}
}

void queue_read_deq(struct queue_read *read){
	if(read == NULL){
		return;
	}
	
	if(read->value == SYMBOL_COUNT){
		return;
	}
	
	if(read->subqueue == NULL || read->subqueue->value == SYMBOL_COUNT){
		read->value = SYMBOL_COUNT;
	}else{
		queue_read_deq(read->subqueue);
	}
}

bool queue_read_complete(const struct queue_read *read){
	if(read == NULL){
		return 1;
	}
	
	return (read->value != SYMBOL_COUNT) && queue_read_complete(read->subqueue);
}

enum queue_read_mode queue_read_mode(const struct queue_read *read){
	if(read == NULL){
		return QUEUE_READ_IDEMPOTENT;
	}
	
	if(read->subqueue == NULL){
		return read->mode;
	}
	
	return queue_read_mode(read->subqueue);
}

symb queue_read_value(const struct queue_read *read){
	return read->value;
}

// ------------------------------------------------------------ ||

void queue_read_update(struct queue_read *read,int in,bool is_switching,void (*on_submit)(enum queue_read_mode)){
	if(is_switching){
		queue_read_init(read,QUEUE_READ_IDEMPOTENT);
		
	}else{
		enum queue_read_mode read_mode = queue_read_mode(read);
		
		if(read_mode == QUEUE_READ_IDEMPOTENT){
			int trigger_add    = read->io_conf->trigger_add;
			int trigger_remove = read->io_conf->trigger_remove;
			
			if(trigger_add != TRIGGER_DISABLED && in == trigger_add){
				queue_read_init(read,QUEUE_READ_ADD);
				
			}else if(trigger_remove != TRIGGER_DISABLED && in == trigger_remove){
				queue_read_init(read,QUEUE_READ_REMOVE);
				
			}
		}else{
			switch(in){
			case '\b':
				queue_read_deq(read);
				
				break;
			case '\t':
			case '\n':
				if(!queue_read_complete(read)){
					break;
				}
				
				on_submit(read_mode);
				
				if(in == '\t' && read->io_conf->triggers_chain){
					queue_read_init(read,read_mode);
				}else{
					queue_read_init(read,QUEUE_READ_IDEMPOTENT);
				}
				
				break;
			case '`':
				queue_read_init(read,QUEUE_READ_IDEMPOTENT);
				
				break;
			default:
				if(is_symbol((char)in)){
					queue_read_enq(read,symbol((char)in));
				}
				
				break;
			}
		}
	}
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

bool is_current;

static void queue_read_draw_value(const struct queue_read *read){
	if(read == NULL){
		return;
	}
	
	if(is_current){
		attrset(A_REVERSE);
	}
	
	addch(ascii(read->value));
	
	if(is_current){
		attrset(A_NORMAL);
	}
	
	if(read->subqueue != NULL){
		addch(',');
	}
	
	is_current = (read->value != SYMBOL_COUNT && read->subqueue != NULL && read->subqueue->value == SYMBOL_COUNT);
	queue_read_draw_value(read->subqueue);
}

void queue_read_draw(const struct queue_read *read){
	enum queue_read_mode read_mode = queue_read_mode(read);
	
	if(read_mode == QUEUE_READ_IDEMPOTENT){
		return;
	}
	
	// Prefix ------------
	char prefix = PREFIX_DISABLED;
	
	switch(read_mode){
	case QUEUE_READ_ADD:
		prefix = read->io_conf->prefix_add;
		
		break;
	case QUEUE_READ_REMOVE:
		prefix = read->io_conf->prefix_remove;
		
		break;
	case QUEUE_READ_IDEMPOTENT:
	default:
		break;
	}
	
	if(prefix != PREFIX_DISABLED){
		addch(' ');
		addch(prefix);
		addch(' ');
	}
	
	// Queue contents ----
	bool bracket = read->io_conf->bracket;
	bool paranthesize = read->io_conf->paranthesize;
	
	if(bracket){
		addch('{');
	}
	
	if(paranthesize){
		addch('(');
	}
	
	is_current = (read->value == SYMBOL_COUNT);
	queue_read_draw_value(read);
	
	if(paranthesize){
		addch(')');
	}
	
	if(bracket){
		addch('}');
	}
}