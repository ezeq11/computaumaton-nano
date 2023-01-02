#ifndef AUTOMATON_INCLUDED
	#include "unsigned.h"
	
	#include "set.h"
	#include "product.h"
	
	struct fsa{
		struct set S;
		struct set Q;
		
		uint q0;
		struct product D0,D1,D2;
		
		struct set F;
	};
	
	#define FSA_INIT(FSA) {\
		SET_INIT(NULL,&(FSA.F),&(FSA.D0)),\
		SET_INIT(NULL,NULL    ,&(FSA.D0)),\
		\
		0,\
		PRODUCT_INIT(&(FSA.Q),&(FSA.D1)),\
		PRODUCT_INIT(&(FSA.S),&(FSA.D2)),\
		PRODUCT_INIT(&(FSA.Q),NULL     ),\
		\
		SET_INIT(&(FSA.S),NULL,NULL)\
	}
	
	#define AUTOMATON_INCLUDED
#endif