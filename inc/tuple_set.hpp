#ifndef TUPLE_SET_INCLUDED
#define TUPLE_SET_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	class tuple_set{
	private:
		enum read_type{
			READ_IDEMPOTENT,
			READ_ADD,
			READ_REMOVE,
			READ_SET
		};
		
		enum draw_type{
			DRAW_INVALID,
			DRAW_TUPLE,
			DRAW_HORIZONTAL_SINGLE,
			DRAW_HORIZONTAL_MULTI,
			DRAW_VERTICAL
		};
		
		// Fields ----------------------------------------------------- ||
		const uint NONVAR_N,N,BLOCK_SIZE;
		const uint WRAP_SIZE,TUPLE_PRINT_WIDTH; // derived
		
		const char prefix_1,prefix_2;
		
		// State
		read_type state;
		
		// Edit data
		uint pos;
		const tuple_set * const * const supersets; // array expected to have at least N elements
		symb * const buffer; // ..................... array expected to have at least N elements
		
		// Set data
		uint len;
		symb * const block; //....................... array expected to have at least N * BLOCK_SIZE elements
		
		// Draw parameters
		bool is_visible;
		
		// Redraw data
		mutable bool redraw_component;
		mutable bool redraw_read;
		
		mutable uint prev_height;
		
		// Edit methods ----------------------------------------------- ||
		void init_read(read_type new_state);
		
		static const tuple_set *containing_superset;
		static symb contained_val;
		
		void remove_if(bool (tuple_set::*remove_tuple)(uint) const);
		bool tuple_equals_buffer(uint i) const;
		bool tuple_contains(uint i) const;
		
		void on_add();
		void on_remove();
		void on_set();
		void on_clear();
		
		// Draw methods ----------------------------------------------- ||
		uint contents_height(draw_type draw_mode) const;
		uint height(draw_type draw_mode) const;
		
		void print_tuple(const symb *tuples,uint i) const;
		void draw_component(int y,draw_type draw_mode) const;
		void draw_read(int y,draw_type draw_mode) const;
		
	public:
		tuple_set(uint INIT_NONVAR_N,uint INIT_N,uint INIT_BLOCK_SIZE,char init_prefix_1,char init_prefix_2,const tuple_set * const * init_supersets,symb *init_buffer,symb *init_block);
		
		void edit(int in);
		bool contains(symb val) const;
		
		void set_visibility(bool new_visibility);
		int draw(int y) const;
	};
	
#endif