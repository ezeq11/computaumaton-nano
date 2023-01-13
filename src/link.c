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
				
				is_contained = is_contained && (super->head->type->contains(super->object,val));
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
		init();
	}
	
	links_invoke(head->next,action);
	
	if(complete != NULL){
		links_invoke(head->next,complete);
	}
}

static void chain_invoke_read_sequence(struct link_head *head,void (*init)(),void (*action)(void *,symb),void (*complete)(void *,symb)){
	if(init != NULL){
		init();
	}
	
	links_invoke_read(head->next,action);
	
	if(complete != NULL){
		links_invoke_read(head->next,complete);
	}
}

// ------------------------------------------------------------ ||

static void relations_remove_references(struct link_relation *relations,uint relations_size,struct link *super,symb val){
	for(uint i = 0;i < relations_size;++i){
		if(relations[i].super == super){
			// Variables
			struct link *sub = relations[i].sub;
			struct link_head *head = sub->head;
			const struct chain_type *type = head->type;
			
			// Removal invocation sequence
			type->on_or_remove_init();
			type->on_or_remove(sub->object,val);
			links_invoke_read(head->next,type->on_or_remove_complete);
			
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
	const struct chain_type *type = head->type;
	
	if(head->read == LINK_IDEMPOTENT){
		switch(in){
			case 'u':
			case 'U':
				if(type->on_add != NULL){
					head->read = LINK_ADD;
					links_clear(head->next);
				}
				
				break;
			case '\\':
				if(type->on_and_remove != NULL && type->on_or_remove != NULL){
					head->read = LINK_REMOVE;
					links_clear(head->next);
				}
				
				break;
			case '=':
				if(type->on_set != NULL){
					head->read = LINK_SET;
					links_clear(head->next);
				}
				
				break;
			case '/':
				if(type->on_clear != NULL){
					chain_invoke_sequence(head,
						type->on_clear_init,
						type->on_clear,
						type->on_clear_complete
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
						type->on_add_init,
						type->on_add,
						type->on_add_complete
					);
					
					break;
				case LINK_REMOVE:
					chain_invoke_read_sequence(head,
						type->on_and_remove_init,
						type->on_and_remove,
						type->on_and_remove_complete
					);
					
					links_remove_read_references(head->next,relations,relations_size);
					
					break;
				case LINK_SET:
					chain_invoke_read_sequence(head,
						type->on_set_init,
						type->on_set,
						type->on_set_complete
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

static uint links_size(const struct link *l){
	if(l == NULL){
		return 0;
	}
	
	uint this_size = l->head->type->size(l->object);
	uint next_size = links_size(l->next);
	
	return this_size > next_size ? this_size : next_size;
}

static void link_draw_append_connective(uint depth,const struct link *l){
	if(l->next == NULL){
		return;
	}
	
	const struct link_head *head = l->head;
	
	if(depth >= head->nonvariadic_len){
		return;
	}
	
	if(depth == head->transition_pos){
		if(head->type->paranthesize){
			addch(')');
		}
		
		addch('-');
		addch('>');
		
		if(head->type->paranthesize){
			addch('(');
		}
	}else{
		addch(',');
	}
}

// ------------------------------------------------------------ ||

static bool links_draw_contents_iter_begin(const struct link *l){
	bool this_has_more_to_draw = l->head->type->draw_horizontal_iter_begin(l->object);
	bool next_has_more_to_draw = links_draw_iter_begin(l->next);
	
	return this_has_more_to_draw || next_has_more_to_draw;
}

static bool links_draw_contents_iter_next(uint depth,const struct link *l){
	addch(ascii(l->head->type->draw_horizontal_iter_get(l->object)));
	link_draw_append_connective(depth,l);
	
	bool this_has_more_to_draw = l->head->type->draw_horizontal_iter_seek(l->object);
	bool next_has_more_to_draw = links_draw_contents_iter_next(depth + 1,l->next);
	
	return this_has_more_to_draw || next_has_more_to_draw;
}

static int chain_draw_contents_horizontal(const struct link_head *head,int y,int x){
	const struct type = head->type;
	bool indented = (links_size(head->next) > type->wrap_size);
	
	move(y,x);
	
	if(type->bracket){
		addch('{');
		
		if(!indented){
			addch(' ');
		}
	}
	
	bool draw_more = links_draw_contents_iter_begin(head->next);
	uint column = 0;
	
	while(draw_more){
		if(type->paranthesize){
			addch('(');
		}
		
		draw_more = links_draw_contents_iter_next(0,head->next);
		++column;
		
		if(type->paranthesize){
			addch(')');
		}
		
		if(draw_more){
			addch(',');
			
			if(column < type->wrap_size){
				addch(' ');
			}else{
				++y;
				move(y,x + 2);
				
				column = 0;
			}
		}
	}
	
	if(type->bracket){
		if(indented){
			++y;
			move(y,x);
			
			addch('}');
		}else{
			addch(' ');
			addch('}');
		}
	}
	
	return y + 2;
}

// ------------------------------------------------------------ ||

static void links_draw_contents_get(uint depth,const struct link *l,uint i){
	if(l == NULL){
		return;
	}
	
	addch(ascii(l->head->type->draw_vertical_get(l->object,i)));
	link_draw_append_connective(depth,l);
	
	links_draw_contents_get(depth + 1,l->next,i);
}

static int chain_draw_contents_vertical(const struct link_head *head,int y,int x){
	const struct chain_type *type = head->type;
	
	uint size = links_size(head->next);
	uint height = (size / type->wrap_size) + (size % type->wrap_size > 0);
	
	if(type->bracket){
		move(y,x);
		addch('{');
	}
	
	for(uint r = 0;r < height;++r){
		uint width = r * type->wrap_size <= size ? type->wrap_size : size % type->wrap_size;
		
		if(type->bracket){
			move(y + 1 + r,x + 2);
		}else{
			move(y + r,x);
		}
		
		for(uint c = 0;c < width;++c){
			uint i = r * h + c;
			
			if(type->paranthesize){
				addch('(');
			}
			
			links_draw_contents_get(0,head->next,i);
			
			if(type->paranthesize){
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
	
	if(type->bracket){
		move(y + 1 + height,x);
		addch('}');
		
		return y + 1 + height + 2;
	}else{
		return y + height + 1;
	}
}

// ------------------------------------------------------------ ||

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
			mark = (links_draw_read_prev->read_val != SYMBOL_COUNT);
		}else{
			mark = ((links_draw_read_prev->read_val != SYMBOL_COUNT) && (l->read_val == SYMBOL_COUNT));
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

static int chain_draw_read(const struct link_head *head){
	const struct chain_type *type = head->type;
	
	// Chain
	if(type->bracket){
		addch('{');
		addch(' ');
	}
	
	if(type->paranthesize){
		addch('(');
	}
	
	links_draw_read_prev = NULL;
	links_draw_read(0,head->next);
	
	if(type->paranthesize){
		addch(')');
	}
	
	if(type->bracket){
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

// ------------------------------------------------------------ ||

static int chain_draw_contents(const struct link_head *head,int y,int x){
	const struct chain_type *type = head->type;
	
	if(
		type->draw_horizontal_iter_begin != NULL &&
		type->draw_horizontal_iter_get != NULL &&
		type->draw_horizontal_iter_seek != NULL
	){
		return chain_draw_contents_horizontal(head,y,x);
	}else{
		return chain_draw_contents_vertical(head,y,x);
	}
}

int chain_draw(const struct link_head *head,int y,int x){
	switch(head->read){
	case LINK_IDEMPOTENT:
		y = chain_draw_contents(head,y,x);
		
		break;
	case LINK_ADD:
		y = chain_draw_contents(head,y,x);
		
		addch(' ');
		addch('U');
		addch(' ');
		
		y = chain_draw_read(head);
		
		break;
	case LINK_REMOVE:
		y = chain_draw_contents(head,y,x);
		
		addch(' ');
		addch('\\');
		addch(' ');
		
		y = chain_draw_read(head);
		
		break;
	case LINK_SET:
		move(y,x);
		
		y = chain_draw_read(head);
		
		break;
	}
	
	return y;
}