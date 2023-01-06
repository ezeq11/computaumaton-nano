#ifndef AUTOMATON_INCLUDED
	#include "set.h"
	#include "element.h"
	#include "product.h"
	
	// ------------------------------------------------------------ ||
	
	enum automaton_state{
		AUT_STATE_IDLE,
		AUT_STATE_STEPPING
	};
	
	// ------------------------------------------------------------ ||
	
	enum fsa_focus{
		FSA_FOCUS_S,
		FSA_FOCUS_Q,
		FSA_FOCUS_Q0,
		FSA_FOCUS_D,
		FSA_FOCUS_F,
		
		FSA_FOCUS_COUNT
	};
	
	struct fsa{
		// Program metadata
		enum automaton_state state;
		enum fsa_focus       focus;
		
		// Mathematical definition
		struct set S;
		struct set Q;
		
		struct element q0;
		struct product D0,D1,D2;
		
		struct set F;
	};
	
	#define FSA_INIT(FSA) {\
		AUT_STATE_IDLE,\
		FSA_FOCUS_S,\
		\
		SET_INIT(NULL,NULL    ,NULL     ,&(FSA.D0)),\
		SET_INIT(NULL,&(FSA.F),&(FSA.q0),&(FSA.D0)),\
		\
		ELEMENT_INIT(&(FSA.Q)),\
		PRODUCT_INIT_LINK(&(FSA.Q),&(FSA.D1)),\
		PRODUCT_INIT_LINK(&(FSA.S),&(FSA.D2)),\
		PRODUCT_INIT_TAIL(&(FSA.Q)),          \
		\
		SET_INIT(&(FSA.Q),NULL,NULL,NULL)\
	}
	
	void fsa_update(struct fsa *a,int in);
	void fsa_draw(int y,int x,const struct fsa *a);
	void fsa_draw_help(int x,const struct fsa *a);
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif