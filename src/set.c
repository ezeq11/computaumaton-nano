#include <stddef.h>

#include "set.h"
#include "element.h"
#include "product.h"

const struct link_funcset SET_FUNCSET = {\
	&(interface_set_contains),\
	\
	NULL,                \
	&(interface_set_add),\
	NULL,                \
	\
	NULL,                   \
	&(interface_set_remove),\
	NULL,                   \
	\
	&(interface_set_remove),\
	\
	NULL,\
	NULL,\
	NULL,\
	\
	NULL,\
	NULL,\
	NULL \
}

void set_add(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_add(s->members,(uint)i);
}

void set_remove(struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return;
	}
	
	bit_array_remove(s->members,(uint)i);
}

bool set_contains(const struct set *s,symb i){
	if(i >= SYMBOL_COUNT){
		return 0;
	}
	
	return bit_array_get(s->members,(uint)i);
}

// ------------------------------------------------------------ ||

void interface_set_add(void *s,symb i){
	set_add((struct set *)s,i);
}

void interface_set_remove(void *s,symb i){
	set_remove((struct set *)s,i);
}

bool interface_set_contains(const void *s,symb i){
	return set_contains((const struct set *)s,i);
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

int set_draw(int y,int x,const struct set *s){
	set_i = 0;
	set_size = bit_array_size(s->members,SYMBOL_COUNT);
	
	move(y,x);
	
	addch('{');
	addch(' ');
	bit_array_forall(s->members,SYMBOL_COUNT,&set_draw_member);
	addch(' ');
	addch('}');
	
	return queue_read_draw(&(s->read));
}

int set_nodraw(int y){
	return queue_read_nodraw(y);
}

int set_draw_help(int y,int x,const struct set *s){
	return queue_read_draw_help(y,x,&(s->read));
}