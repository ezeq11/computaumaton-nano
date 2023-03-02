#ifndef TUPLE_SET_INCLUDED
#define TUPLE_SET_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "screen_space.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define SET_BLOCK_SIZE 8
		#define PRODUCT_BLOCK_SIZE 64
	#else
		#define SET_BLOCK_SIZE 32
		#define PRODUCT_BLOCK_SIZE 256
	#endif
	
	#define MAX_N 16
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_layout: a description of a tuple's format
	  
	  TRANSITION_POS
	  -        |
	  - (#,#,#)->(#,#,#####)
	  - 
	  - this determines where the arrow appears in every tuple, and how many values are used for filtering tuples.
	  - setting TRANSITION_POS = 0 effectively disables it (filtering will not work and the arrow will not be drawn)
	  - TRANSITION_POS has a max value of 3 (see tuple_set_operations::filter_apply)
	  
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
	  
	  Please note that the code assumes TRANSITION_POS + NONVAR_COUNT <= N <= MAX_N, and will likely break if this
	  is not the case.
	*/
	
	struct tuple_layout{
		const uint TRANSITION_POS,NONVAR_COUNT,N,BLOCK_SIZE;
		const bool DRAW_VERTICAL;
		const uint WRAP_SIZE,TUPLE_PRINT_WIDTH; // derived
		
		tuple_layout(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,uint INIT_BLOCK_SIZE,bool INIT_DRAW_VERTICAL);
	};
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_set: an array of unique N-tuples
	  
	  -         N = 3          len = 2
	  -        |     |         |
	  - block: (#,#,#),(#,#,#),(_,_,_),(_,_,_),(_,_,_),...
	*/
	
	class tuple_set_operations;
	
	class tuple_set: public screen_space{
		friend tuple_set_operations;
		
	// Fields ------------------------------------------- |
	private:
		tuple_layout * const layout;
		
		const char prefix_1,prefix_2;
		const tuple_set * * const supersets; // array expected to have at least layout->N elements
		
		// Data -------
		uint len;
		symb * const block; //................. array expected to have at least layout->N * layout->BLOCK_SIZE elements
		
		// Draw -------
		bool is_visible;
		bool are_contents_shown;
		symb (*filter_applied)[3];
		
	// Data methods ------------------------------------- |
		static const symb *comparison_buffer;
		static const tuple_set *containing_superset;
		static symb contained_val;
		
		void remove_if(bool (tuple_set::*remove_tuple)(uint) const);
		bool tuple_equals_buffer(uint i) const;
		bool tuple_contains(uint i) const;
		
	public:
		void on_add(const symb *tuple);
		void on_remove(const symb *tuple);
		void on_set(const symb *tuple);
		void on_clear();
		
		void remove_containing(const tuple_set *superset,symb val);
		bool contains(symb val) const;
		uint size() const;
		
	// Draw methods ------------------------------------- |
	private:
		void move_to_header_prefix() const;
		void move_to_read_position() const;
		void move_to_ith_suffix(uint i) const;
		
		void draw_show_contents(bool new_contents_shown);
		void draw_apply_filter(symb (*new_filter)[3]);
		
	public:
		void draw_set_visibility(bool new_visibility);
		
		void demarcate() const;
		void draw() const;
		
	// OOP object management ---------------------------- |
		tuple_set(screen_space *init_next,tuple_layout *init_layout,char init_prefix_1,char init_prefix_2,const tuple_set * * init_supersets,symb *init_block);
		void set_superset(uint i,const tuple_set *superset);
	};
	
	// declared here, but not defined in tuple_set.cpp
	// whoever triggers tuple_set::on_remove() (in this case, automata.cpp's use of tuple_set_operations.edit()) has the responsibility of defining its value
	extern void (*monad_set_on_remove_callback)(const tuple_set *,symb);
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_set_operations: a multiplexer for tuple_set modifications which handles state and draws UI appropriately
	  
	  usage: call switch_to(<tuple_set instance>,<OPERATION>)
	       - then all functions named tuple_set_operations::OPERATION_<name> will be available for use
	*/
	
	class tuple_set_operations{
	public:
		enum operation{
			OPERATION_NIL,
			OPERATION_EDIT,
			OPERATION_FILTER
		};
		
	private:
		enum read{
			READ_IDEMPOTENT,
			READ_ADD,
			READ_REMOVE,
			READ_SET
		};
		
	// Fields ------------------------------------------- |
		tuple_set *focus;
		operation current_operation;
		
		// Edit -------
		read current_read;
		uint pos;
		symb buffer[MAX_N];
		
		// Filter -----
		symb vals[3];
		uint results_count;
		uint nav;
		
	// Methods ------------------------------------------ |
		void clear_edit_indicator() const;
		void clear_edit_read() const;
		void clear_filter_nav() const;
		
		void draw_edit_indicator() const;
		void draw_edit_read() const;
		void draw_filter_nav() const;
		
		void edit_read_init(read new_read);
		
	public:
		tuple_set_operations();
		
		bool switch_available() const;
		void switch_to(tuple_set *new_focus,operation new_operation);
		
		void draw() const;
		
		// Edit -------
		void edit(int in);
		const tuple_set *edit_current_superset() const;
		void edit_print_available_commands() const;
		
		// Filter -----
		void filter_clear();
		void filter_apply(symb val_0,symb val_1,symb val_2);
		uint filter_results() const;
		
		void filter_nav_next();
		void filter_nav_prev();
		const symb *filter_nav_select() const;
	};
	
	// ------------------------------------------------------------ ||
	class set: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb block[SET_BLOCK_SIZE];
		
	public:
		static tuple_layout layout;
		
		set(screen_space *init_next,char init_prefix_1,char init_prefix_2);
	};
	
	// ------------------------------------------------------------ ||
	class element: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb block[1];
		
	public:
		static tuple_layout layout;
		
		element(screen_space *init_next,char init_prefix_1,char init_prefix_2);
		
		bool is_set() const;
		symb get() const;
	};
	
	// ------------------------------------------------------------ ||
	class product: public tuple_set{
	private:
		tuple_layout layout;
		
		const tuple_set *supersets[MAX_N];
		symb block[PRODUCT_BLOCK_SIZE];
		
	public:
		product(screen_space *init_next,uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,char init_prefix_1,char init_prefix_2);
	};
	
#endif