#include <stddef.h>

#include "unsigned.h"
#include "link.h"
#include "set.h"

static struct link *link_prev;

static void links_clear(struct link *l){
	if(l == NULL){
		return;
	}
	
	l->read_val = SYMBOL_COUNT;
	
	links_clear(l->next);
}

static void links_dequeue(struct link *l){
	if(link_prev != NULL && link_prev->read_val != SYMBOL_COUNT && (l == NULL || l->read_val == SYMBOL_COUNT)){
		link_prev->read_val = SYMBOL_COUNT;
		return;
	}
	
	if(l == NULL){
		return;
	}
	
	link_prev = l;
	links_dequeue(l->next);
}

static void links_enqueue(struct link *l,symb val){
	if(l == NULL){
		return;
	}
	
	if(l->read_val == SYMBOL_COUNT){
		if(l->superset == NULL || set_contains(l->superset,val)){
			l->read_val = val;
		}
		
		return;
	}
	
	links_enqueue(l->next,val);
}

static bool links_complete(struct link *l){
	if(l == NULL){
		return 1;
	}
	
	return (l->read_val != SYMBOL_COUNT) && links_complete(l->next);
}

static void links_invoke(struct link *l,void (*f)(void *)){
	if(l == NULL){
		return;
	}
	
	f(l->object);
	
	links_invoke(l->next,f);
}

static void links_invoke_symboled(struct link *l,void (*f)(void *,symb)){
	if(l == NULL){
		return;
	}
	
	f(l->object,l->read_val);
	
	links_invoke_symboled(l->next,f);
}

void links_update(struct link_head *head,int in){
	if(head->read == LINK_IDEMPOTENT){
		switch(in){
			case 'u':
			case 'U':
				if(head->on_add != NULL){
					head->read = LINK_ADD;
					links_clear(head->next);
				}
				
				break;
			case '\\':
				if(head->on_remove != NULL){
					head->read = LINK_REMOVE;
					links_clear(head->next);
				}
				
				break;
			case '=':
				if(head->on_set != NULL){
					head->read = LINK_SET;
					links_clear(head->next);
				}
				
				break;
			case '/':
				if(head->on_clear != NULL){
					links_invoke(head->next,head->on_clear);
				}
				
				break;
		}
	}else{
		switch(in){
		case '\b':
		case 0x7f:
			link_prev = NULL;
			links_dequeue(head->next);
			
			break;
		case '\t':
		case '\n':
			if(!links_complete(head->next)){
				break;
			}
			
			switch(head->read){
				case LINK_IDEMPOTENT:
					break;
				case LINK_ADD:
					links_invoke_symboled(head->next,head->on_add);
					
					break;
				case LINK_REMOVE:
					links_invoke_symboled(head->next,head->on_remove);
					
					break;
				case LINK_SET:
					links_invoke_symboled(head->next,head->on_set);
					
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
				links_enqueue(head->next,symbol((char)in));
			}
			
			break;
		}
	}
}