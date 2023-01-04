#ifndef AUTOMATON_INCLUDED
	#include "set.h"
	#include "element.h"
	#include "product.h"
	
	// ------------------------------------------------------------ ||
	
	enum automaton_state{
		AUT_STATE_IDLE,
		AUT_STATE_STEPPING
	};
	
	enum automaton_edit{
		AUT_EDIT_IDEMPOTENT,
		AUT_EDIT_UNION,
		AUT_EDIT_DIFFERENCE
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
		enum automaton_edit  edit;
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
		AUT_EDIT_IDEMPOTENT,\
		FSA_FOCUS_D,\
		\
		SET_INIT(NULL,&(FSA.F),&(FSA.D0)),\
		SET_INIT(NULL,NULL    ,&(FSA.D0)),\
		\
		ELEMENT_INIT(&(FSA.S)),\
		PRODUCT_INIT_LINK(&(FSA.Q),&(FSA.D1)),\
		PRODUCT_INIT_LINK(&(FSA.S),&(FSA.D2)),\
		PRODUCT_INIT_TAIL(&(FSA.Q)),          \
		\
		SET_INIT(&(FSA.Q),NULL,NULL)\
	}
	
	void fsa_update(struct fsa *a,int in);
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif