#ifndef DRAW_INCLUDED
	#include "set.h"
	#include "product.h"
	
	void draw_set(int y,int x,struct set *s);
	void draw_product(int y,int x,struct product *p,char prefix,int max_rows);
	
	#define DRAW_INCLUDED
#endif