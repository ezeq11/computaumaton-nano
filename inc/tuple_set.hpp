#ifndef TUPLE_SET_INCLUDED
#define TUPLE_SET_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define SET_BLOCK_SIZE 8
		#define PRODUCT_BLOCK_SIZE 64
	#else
		#define SET_BLOCK_SIZE 32
		#define PRODUCT_BLOCK_SIZE 256
	#endif
	
	#define MAX_PRODUCT_N 16
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_config: a description of a tuple's properties
	  
	  TRANSITION_POS
	  -        |
	  - (#,#,#)->(#,#,#####)
	  - 
	  - this determines where the arrow appears in every tuple, and how many values are used for filtering tuples.
	  - setting TRANSITION_POS = 0 effectively disables it (filtering will not work and the arrow will not be drawn)
	  - TRANSITION_POS has a max value of 3 (see tuple_set::filter_apply)
	  
	  NONVAR_COUNT
	  -          |   |
	  - (#,#,#)->(#,#,#####)
	  - 
	  - this determines how many values after the transition position are non-variadic.
	  - that is, they are required for read input, and will draw with commas after them.
	  
	  N
	  - |                  |
	  - (#,#,#)->(#,#,#####)
	  - 
	  - this determines the number of symbols in the entire tuple, including filterable, nonvariadic, and variadic.
	  - thus, the maximum length of variadic input allowed for a single tuple is N - NONVAR_COUNT - TRANSITION_POS.
	  
	  Please note that the code assumes TRANSITION_POS + NONVAR_COUNT <= N, and will likely break if this is not
	  the case.
	*/
	
	struct tuple_config{
		const uint _TRANSITION_POS,_NONVAR_COUNT,_N,_BLOCK_SIZE;
		const uint _WRAP_SIZE,_TUPLE_PRINT_WIDTH; // derived
		
		tuple_config(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,uint INIT_BLOCK_SIZE);
	};
	
	// ------------------------------------------------------------ ||
	/*
	  filter_store: an optional storage space for filter values and results
	*/
	
	struct filter_store{
		bool applied;
		uint results_count;
		
		symb vals[3];
		uint nav;
	};
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_set: an array of unique N-tuples
	  
	  -       N = 3          len = 2
	  -      |     |         |
	  block: (#,#,#),(#,#,#),(_,_,_),(_,_,_),(_,_,_),...
	*/
	
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
		tuple_config * const config;
		filter_store * const filter;
		const char prefix_1,prefix_2;
		
		// State
		read_type state;
		
		// Edit data
		uint pos;
		const tuple_set * * const supersets; // array expected to have at least config->N elements
		symb * const buffer; // ............... array expected to have at least config->N elements
		
		// Set data
		uint len;
		symb * const block; //................. array expected to have at least config->N * config->BLOCK_SIZE elements
		
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
		
	protected:
		uint size() const;
		
	public:
		tuple_set(tuple_config *init_config,filter_store *init_filter,char init_prefix_1,char init_prefix_2,const tuple_set * * init_supersets,symb *init_buffer,symb *init_block);
		
		void             set_superset(uint i,const tuple_set *superset);
		const tuple_set *get_superset_current() const;
		
		// Edit methods ----------------------------------------------- ||
		void remove_containing(const tuple_set *superset,symb val);
		void edit(int in);
		
		bool is_amid_edit() const;
		bool contains(symb val) const;
		
		// Filter methods --------------------------------------------- ||
		void filter_clear();
		void filter_apply(symb filter_1,symb filter_2,symb filter_3);
		uint filter_results() const;
		
		void        filter_nav_next();
		void        filter_nav_prev();
		const symb *filter_nav_select() const;
		
		// Draw methods ----------------------------------------------- ||
		void force_redraw();
		void set_visibility(bool new_visibility);
		
		int draw(int y) const;
	};
	
	// declared here, but not defined in tuple_set.cpp
	// whoever uses tuple_set::edit() (in this case, automata.cpp) has the responsibility of defining its value
	extern void (*monad_set_on_remove_callback)(const tuple_set *,symb);
	
	// ------------------------------------------------------------ ||
	class set: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb buffer[1];
		
		symb block[SET_BLOCK_SIZE];
		
	public:
		static tuple_config config;
		
		set(char init_prefix_1,char init_prefix_2);
	};
	
	// ------------------------------------------------------------ ||
	class element: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb buffer[1];
		
		symb block[1];
		
	public:
		static tuple_config config;
		
		element(char init_prefix_1,char init_prefix_2);
		
		bool is_set() const;
		symb get() const;
	};
	
	// ------------------------------------------------------------ ||
	class product: public tuple_set{
	private:
		tuple_config config;
		filter_store filter;
		
		const tuple_set *supersets[MAX_PRODUCT_N];
		symb buffer[MAX_PRODUCT_N];
		
		symb block[PRODUCT_BLOCK_SIZE];
		
	public:
		product(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,char init_prefix_1,char init_prefix_2);
	};
	
#endif