#ifndef DRAW_INCLUDED
	#include "set.h"
	#include "product.h"
	
	#include "automaton.h"
	
	void draw_set(int y,int x,struct set *s);
	void draw_product(int y,int x,struct product *p,int max_rows,enum automaton_edit edit);
	
	void draw_fsa(int y,int x,struct fsa *a);
	
	#define DRAW_INCLUDED
#endif