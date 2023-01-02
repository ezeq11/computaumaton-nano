#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "unsigned.h"
#include "charset.h"
#include "draw.h"

uint set_i;
uint set_size;

static void draw_set_member(uint i){
	addch(ascii(i));
	++set_i;
	
	if(set_i < set_size){
		addch(',');
		addch(' ');
	}
}

void draw_set(int y,int x,struct set *s){
	set_i = 0;
	set_size = bit_array_size(s->members,CHARSET_SIZE);
	
	move(y,x);
	
	addch('{');
	addch(' ');
	bit_array_forall(s->members,CHARSET_SIZE,&draw_set_member);
	addch(' ');
	addch('}');
}

static void draw_tuple_member(uchar val,uchar is_tuple_end){
	addch(ascii(val));
	
	if(!is_tuple_end){
		addch(',');
	}
}

void draw_product(int y,int x,struct product *p,int max_rows){
	uint prod_size = product_size(p);
	
	/*
	  max_rows = 3
	  prod_size = 7
	  
	  row  0: p_0 p_3 p_6
	  row  1: p_1 p_4
	  row  2: p_2 p_5
	*/
	
	move(y,x);
	addch('{');
	
	for(uint row = 0;row < max_rows;++row){
		uint row_width = (prod_size / max_rows) + (row < (prod_size % max_rows));
		
		if(row_width == 0){
			continue;
		}
		
		move(y + 1 + row,x);
		addch(' ');
		addch(' ');
		
		for(uint column = 0;column < row_width;++column){
			uint i = row + max_rows * column;
			
			addch('(');
			product_fortuple(p,i,&draw_tuple_member);
			addch(')');
			
			if(i + 1 < prod_size){
				addch(',');
				addch(' ');
			}
		}
	}
	
	move(y + 1 + max_rows,x);
	addch('}');
}