#ifndef AUTOMATON_INCLUDED
	#include "element.h"
	#include "set.h"
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
		.link_heads = {\
			[FSA_FOCUS_S ] = { .type = &(SET_TYPE    ), .nonvariadic_len = 1, .transition_pos = 1, .read = LINK_READ_INIT, .next = &(FSA.link_S ) },\
			[FSA_FOCUS_Q ] = { .type = &(SET_TYPE    ), .nonvariadic_len = 1, .transition_pos = 1, .read = LINK_READ_INIT, .next = &(FSA.link_Q ) },\
			[FSA_FOCUS_Q0] = { .type = &(ELEMENT_TYPE), .nonvariadic_len = 1, .transition_pos = 1, .read = LINK_READ_INIT, .next = &(FSA.link_q0) },\
			[FSA_FOCUS_D ] = { .type = &(PRODUCT_TYPE), .nonvariadic_len = 3, .transition_pos = 1, .read = LINK_READ_INIT, .next = &(FSA.link_D0) },\
			[FSA_FOCUS_F ] = { .type = &(SET_TYPE    ), .nonvariadic_len = 1, .transition_pos = 1, .read = LINK_READ_INIT, .next = &(FSA.link_F ) } \
		},\
		\
		.link_S  = { .head = &(FSA.links_heads[FSA_FOCUS_S ]), .object = &(FSA.S ), .read_val = LINK_READ_VAL_INIT, .next = NULL           },\
		.link_Q  = { .head = &(FSA.links_heads[FSA_FOCUS_Q ]), .object = &(FSA.Q ), .read_val = LINK_READ_VAL_INIT, .next = NULL           },\
		.link_q0 = { .head = &(FSA.links_heads[FSA_FOCUS_Q0]), .object = &(FSA.q0), .read_val = LINK_READ_VAL_INIT, .next = NULL           },\
		.link_D0 = { .head = &(FSA.links_heads[FSA_FOCUS_D ]), .object = &(FSA.D0), .read_val = LINK_READ_VAL_INIT, .next = &(FSA.link_D1) },\
		.link_D1 = { .head = &(FSA.links_heads[FSA_FOCUS_D ]), .object = &(FSA.D1), .read_val = LINK_READ_VAL_INIT, .next = &(FSA.link_D2) },\
		.link_D2 = { .head = &(FSA.links_heads[FSA_FOCUS_D ]), .object = &(FSA.D2), .read_val = LINK_READ_VAL_INIT, .next = NULL           },\
		.link_F  = { .head = &(FSA.links_heads[FSA_FOCUS_F ]), .object = &(FSA.F ), .read_val = LINK_READ_VAL_INIT, .next = NULL           },\
		\
		.link_relations = {\
			{ .super = &(FSA.link_Q), .sub = &(FSA.link_q0) },\
			{ .super = &(FSA.link_Q), .sub = &(FSA.link_D0) },\
			{ .super = &(FSA.link_S), .sub = &(FSA.link_D1) },\
			{ .super = &(FSA.link_Q), .sub = &(FSA.link_D2) },\
			{ .super = &(FSA.link_Q), .sub = &(FSA.link_F ) } \
		},\
		\
		.S  = SET_INIT,    /
		.Q  = SET_INIT,    /
		.q0 = ELEMENT_INIT,/
		.D0 = PRODUCT_INIT,/
		.D1 = PRODUCT_INIT,/
		.D2 = PRODUCT_INIT,/
		.F  = SET_INIT     /
	}
	
	void fsa_update(struct fsa *a,int in);
	void fsa_draw(int y,int x,const struct fsa *a);
	void fsa_draw_help(int x,const struct fsa *a);
	
	// ------------------------------------------------------------ ||
	
	#define AUTOMATON_INCLUDED
#endif