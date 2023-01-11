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
	
	struct fsa{
		// Link structures
		struct link_head link_heads[FSA_FOCUS_COUNT];
		
		// ---
		struct link link_S;
		struct link link_Q;
		
		struct link link_q0;
		struct link link_D0;
		struct link link_D1;
		struct link link_D2;
		
		struct link link_F;
		// ---
		
		struct link_relation link_relations[5];
		
		// Mathematical definition
		set S;
		set Q;
		
		element q0;
		product D0,D1,D2;
		
		set F;
	};
	
	#define FSA_INIT(FSA) {\
		{\
			LINK_HEAD_INIT_SET    (&(FSA.link_S )),\
			LINK_HEAD_INIT_SET    (&(FSA.link_Q )),\
			LINK_HEAD_INIT_ELEMENT(&(FSA.link_q0)),\
			LINK_HEAD_INIT_PRODUCT(&(FSA.link_D0)),\
			LINK_HEAD_INIT_SET    (&(FSA.link_F )) \
		},\
		\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_S ,&(FSA.S ),NULL          ),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_Q ,&(FSA.Q ),NULL          ),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_Q0,&(FSA.q0),NULL          ),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_D ,&(FSA.D0),&(FSA.link_D1)),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_D ,&(FSA.D1),&(FSA.link_D2)),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_D ,&(FSA.D2),NULL          ),\
		LINK_INIT(FSA.link_heads + FSA_FOCUS_F ,&(FSA.F ),NULL          ),\
		\
		{\
			LINK_RELATION_INIT(&(FSA.link_Q),&(FSA.link_q0)),\
			LINK_RELATION_INIT(&(FSA.link_Q),&(FSA.link_D0)),\
			LINK_RELATION_INIT(&(FSA.link_S),&(FSA.link_D1)),\
			LINK_RELATION_INIT(&(FSA.link_Q),&(FSA.link_D2)),\
			LINK_RELATION_INIT(&(FSA.link_Q),&(FSA.link_F )) \
		},\
		\
		SET_INIT,    /
		SET_INIT,    /
		ELEMENT_INIT,/
		PRODUCT_INIT,/
		PRODUCT_INIT,/
		PRODUCT_INIT,/
		SET_INIT     /
	}
	
	void fsa_update(struct fsa *a,int in);
	void fsa_draw(int y,int x,const struct fsa *a);
	void fsa_draw_help(int x,const struct fsa *a);
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif