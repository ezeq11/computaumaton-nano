#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "charset.h"
#include "set.h"
#include "draw.h"

struct set s = SET_INIT(NULL,NULL,NULL);

struct product p1 = PRODUCT_INIT(&s,NULL);
struct product p0 = PRODUCT_INIT(&s,&p1);

void p_add(char a,char b){
	product_q_enqueue(&p0,charset(a));
	product_q_enqueue(&p0,charset(b));
	product_q_add(&p0);
}

int main(){
	set_add(&s,charset('a'));
	set_add(&s,charset('e'));
	set_add(&s,charset('i'));
	set_add(&s,charset('o'));
	set_add(&s,charset('u'));
	
	p_add('a','e');
	p_add('a','o');
	p_add('e','u');
	p_add('e','a');
	p_add('e','u');
	p_add('e','o');
	p_add('u','u');
	p_add('u','a');
	p_add('u','e');
	p_add('o','i');
	p_add('u','o');
	
	initscr();
	
	draw_set(3,5,&s);
	draw_product(5,5,&p0,7);
	
	refresh();
	getch();
	endwin();
	
	return EXIT_SUCCESS;
}