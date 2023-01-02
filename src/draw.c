#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "unsigned.h"
#include "charset.h"
#include "draw.h"

uint set_size;

static void draw_set_member(uint i){
	addch(ascii(i));
	
	if(i + 1 < set_size){
		addch(',');
	}
}

void draw_set(int y,int x,struct set *s){
	set_size = bit_array_size(s->members,CHARSET_SIZE);
	
	move(y,x);
	
	addch('{');
	bit_array_forall(s->members,CHARSET_SIZE,&draw_set_member);
	addch('}');
}