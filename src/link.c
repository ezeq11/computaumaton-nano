#include <stddef.h>

#include "link.h"

// ------------------------------------------------------------ ||

static void links_clear(struct link *l){
	if(l == NULL){
		return;
	}
	
	l->read_val = SYMBOL_COUNT;
	
	links_clear(l->next);
}

static void links_dequeue(struct link *l){
	if(l == NULL){
		return;
	}
	
	if(l->read_val != SYMBOL_COUNT){
		if(l->next == NULL || l->next->read_val == SYMBOL_COUNT){
			l->read_val = SYMBOL_COUNT;
			
			return;
		}
	}
	
	links_dequeue(l->next);
}

static void links_enqueue(struct link *l,symb val,struct link_relation *relations,uint relations_size){
	if(l == NULL){
		return;
	}
	
	if(l->read_val == SYMBOL_COUNT){
		bool is_contained = 1;
		
		for(uint i = 0;i < relations_size;++i){
			if(relations[i].sub == l){
				struct link *super = relations[i].super;
				
				is_contained = is_contained && (*(super->head->funcset->contains))(super->object,val);
			}
		}
		
		if(is_contained){
			l->read_val = val;
		}
		
		return;
	}
	
	links_enqueue(l->next,val);
}

static bool links_complete(uint depth,const struct link *l){
	if(l == NULL){
		return 1;
	}
	
	if(depth >= l->head->nonvariadic_len){
		return 1;
	}
	
	return (l->read_val != SYMBOL_COUNT) && links_complete(depth + 1,l->next);
}

// ------------------------------------------------------------ ||

static void links_invoke(struct link *l,void (*f)(void *)){
	if(l == NULL){
		return;
	}
	
	f(l->object);
	
	links_invoke(l->next,f);
}

static void links_invoke_read(struct link *l,void (*f)(void *,symb)){
	if(l == NULL){
		return;
	}
	
	f(l->object,l->read_val);
	
	links_invoke_read(l->next,f);
}

static void chain_invoke_sequence(struct link_head *head,void (*init)(),void (*action)(void *),void (*complete)(void *)){
	if(init != NULL){
		(*init)();
	}
	
	links_invoke(head->next,action);
	
	if(complete != NULL){
		links_invoke(head->next,complete);
	}
}

static void chain_invoke_read_sequence(struct link_head *head,void (*init)(),void (*action)(void *,symb),void (*complete)(void *)){
	if(init != NULL){
		(*init)();
	}
	
	links_invoke_read(head->next,action);
	
	if(complete != NULL){
		links_invoke(head->next,complete);
	}
}

// ------------------------------------------------------------ ||

static void relations_remove_references(struct link_relation *relations,uint relations_size,struct link *super,symb val){
	for(uint i = 0;i < relations_size;++i){
		if(relations[i].super == super){
			// Variables
			struct link *sub = relations[i].sub;
			struct link_head *head = sub->head;
			const struct link_funcset * const funcset = head->funcset;
			
			// Removal invocation sequence
			funcset->on_remove_init();
			funcset->on_or_remove(sub->object,val);
			links_invoke(head->next,funcset->on_remove_complete);
			
			// Recurse to cascade removal down the relations tree
			relations_remove_references(relations,relations_size,sub,val);
		}
	}
}

static void links_remove_read_references(struct link *l,struct link_relation *relations,uint relations_size){
	if(l == NULL){
		return;
	}
	
	relations_remove_references(relations,relations_size,l,l->read_val);
	
	links_remove_read_references(l->next,relations,relations_size);
}

// ------------------------------------------------------------ ||

void chain_update(struct link_head *head,int in,struct link_relation *relations,uint relations_size){
	const struct link_funcset *funcset = head->funcset;
	
	if(head->read == LINK_IDEMPOTENT){
		switch(in){
			case 'u':
			case 'U':
				if(funcset->on_add != NULL){
					head->read = LINK_ADD;
					links_clear(head->next);
				}
				
				break;
			case '\\':
				if(funcset->on_and_remove != NULL && funcset->on_or_remove != NULL){
					head->read = LINK_REMOVE;
					links_clear(head->next);
				}
				
				break;
			case '=':
				if(funcset->on_set != NULL){
					head->read = LINK_SET;
					links_clear(head->next);
				}
				
				break;
			case '/':
				if(funcset->on_clear != NULL){
					chain_invoke_sequence(head,
						funcset->on_clear_init,
						funcset->on_clear,
						funcset->on_clear_complete
					);
				}
				
				break;
		}
	}else{
		switch(in){
		case '\b':
		case 0x7f:
			links_dequeue(head->next);
			
			break;
		case '\t':
		case '\n':
			if(!links_complete(0,head->next)){
				break;
			}
			
			switch(head->read){
				case LINK_IDEMPOTENT:
					break;
				case LINK_ADD:
					chain_invoke_read_sequence(head,
						funcset->on_add_init,
						funcset->on_add,
						funcset->on_add_complete
					);
					
					break;
				case LINK_REMOVE:
					chain_invoke_read_sequence(head,
						funcset->on_remove_init,
						funcset->on_and_remove,
						funcset->on_remove_complete
					);
					
					links_remove_read_references(head->next,relations,relations_size);
					
					break;
				case LINK_SET:
					chain_invoke_read_sequence(head,
						funcset->on_set_init,
						funcset->on_set,
						funcset->on_set_complete
					);
					
					break;
			}
			
			if(in == '\t' && (head->read == LINK_ADD || head->read == LINK_REMOVE)){
				links_clear(head->next);
			}else{
				head->read = LINK_IDEMPOTENT;
			}
			
			break;
		case '`':
			head->read = LINK_IDEMPOTENT;
			
			break;
		default:
			if(is_symbol((char)in)){
				links_enqueue(head->next,symbol((char)in),relations,relations_size);
			}
			
			break;
		}
	}
}

// ------------------------------------------------------------ ||

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

// ------------------------------------------------------------ ||

static void link_draw_append_connective(uint depth,const struct link *l){
	if(l->next == NULL){
		return;
	}
	
	if(depth >= l->head->nonvariadic_len){
		return;
	}
	
	if(depth == l->head->transition_pos){
		if(l->head->paranthesize){
			addch(')');
		}
		
		addch('-');
		addch('>');
		
		if(l->head->paranthesize){
			addch('(');
		}
	}else{
		addch(',');
	}
}

static void links_draw_contents(uint depth,const struct link *l,uint i){
	if(l == NULL){
		return;
	}
	
	addch(ascii(l->head->funcset->get(l->object,i)));
	link_draw_append_connective(depth,l);
	
	links_draw_contents(depth + 1,l->next,i);
}

static const struct link *links_draw_read_prev;

static int links_read_cursor_y;
static int links_read_cursor_x;
static bool links_read_cursor_end;

static void links_draw_read(uint depth,const struct link *l){
	// Mark cursor
	bool mark = 0;
	
	if(links_draw_read_prev == NULL){
		if(l == NULL){
			mark = 1;
		}else{
			mark = (l->read_val == SYMBOL_COUNT);
		}
	}else{
		if(l == NULL){
			mark = (links_draw_read->read_val != SYMBOL_COUNT);
		}else{
			mark = ((links_draw_read->read_val != SYMBOL_COUNT) && (l->read_val == SYMBOL_COUNT));
		}
	}
	
	if(mark){
		getyx(stdscr,links_read_cursor_y,links_read_cursor_x);
		++links_read_cursor_y;
		
		links_read_cursor_end = (l == NULL);
	}
	
	if(l == NULL){
		return;
	}
	
	// Draw element
	addch(ascii(l->read_val));
	link_draw_append_connective(depth,l);
	
	// Recurse
	links_draw_read_prev = l;
	links_draw_read(depth + 1,l->next);
}

// ------------------------------------------------------------ ||

static uint links_size(const struct link *l){
	if(l == NULL){
		return 0;
	}
	
	uint this_size = l->head->funcset->size(l->object);
	uint next_size = links_size(l->next);
	
	return this_size > next_size ? this_size : next_size;
}

static int chain_draw_contents(const struct link_head *head,int y,int x){
	uint size = links_size(head->next);
	uint height;
	
	if(head->wrap_sideways){
		height = size > head->wrap_size ? head->wrap_size : size;
	}else{
		height = (size / head->wrap_size) + (size % head->wrap_size > 0);
	}
	
	if(head->bracket){
		move(y,x);
		addch('{');
	}
	
	for(uint r = 0;r < height;++r){
		uint width;
		
		if(head->wrap_sideways){
			width = (size / height) + (r < (size % height));
		}else{
			width = r * head->wrap_size <= size ? head->wrap_size : size % head->wrap_size;
		}
		
		if(head->bracket){
			move(y + 1 + r,x + 2);
		}else{
			move(y + r,x);
		}
		
		for(uint c = 0;c < width;++c){
			uint i;
			
			if(head->wrap_sideways){
				i = c * h + r;
			}else{
				i = r * h + c;
			}
			
			if(head->paranthesize){
				addch('(');
			}
			
			links_draw_contents(0,head->next,i);
			
			if(head->paranthesize){
				addch(')');
			}
			
			if(i + 1 < size){
				addch(',');
			}
			
			if(c + 1 < width){
				addch(' ');
			}
		}
	}
	
	if(head->bracket){
		move(y + 1 + height,x);
		addch('}');
		
		return y + 1 + height + 2;
	}else{
		return y + height + 1;
	}
}

static int chain_draw_read(const struct link_head *head){
	// Chain
	if(head->bracket){
		addch('{');
		addch(' ');
	}
	
	if(head->paranthesize){
		addch('(');
	}
	
	links_draw_read_prev = NULL;
	links_draw_read(0,head->next);
	
	if(head->paranthesize){
		addch(')');
	}
	
	if(head->bracket){
		addch(' ');
		addch('}');
	}
	
	// Cursor
	move(links_read_cursor_y,links_read_cursor_x);
	
	if(links_read_cursor_end){
		addch('<')
	}else{
		addch('^';)
	}
	
	return links_read_cursor_y + 1;
}

int chain_draw(const struct link_head *head,int y,int x){
	int dy = y;
	
	switch(head->read){
	case LINK_IDEMPOTENT:
		dy = chain_draw_contents(head,dy,x);
		
		break;
	case LINK_ADD:
		dy = chain_draw_contents(head,dy,x);
		
		addch(' ');
		addch('U');
		addch(' ');
		
		dy = chain_draw_read(head);
		
		break;
	case LINK_REMOVE:
		dy = chain_draw_contents(head,dy,x);
		
		addch(' ');
		addch('\\');
		addch(' ');
		
		dy = chain_draw_read(head);
		
		break;
	case LINK_SET:
		move(dy,x);
		
		dy = chain_draw_read(head);
		
		break;
	}
	
	return dy;
}