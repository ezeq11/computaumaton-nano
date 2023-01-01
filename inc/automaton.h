#ifndef AUTOMATON_INCLUDED
	#include "unsigned.h"
	
	#include "alphabet.h"
	#include "product.h"
	
	struct fsa{
		struct alphabet S;
		struct alphabet Q;
		
		uint q0;
		struct product D0,D1,D2;
		
		struct alphabet F;
	};
	
	#define FSA_INIT(FSA) {\
		ALPHABET_INIT(NULL,&(FSA.F),&(FSA.D0)),\
		ALPHABET_INIT(NULL,NULL    ,&(FSA.D0)),\
		\
		0,\
		PRODUCT_INIT(&(FSA.Q),&(FSA.D1)),\
		PRODUCT_INIT(&(FSA.S),&(FSA.D2)),\
		PRODUCT_INIT(&(FSA.Q),NULL     ),\
		\
		ALPHABET_INIT(&(FSA.S),NULL,NULL)\
	}
	
	#define AUTOMATON_INCLUDED
#endif