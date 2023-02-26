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
	
	#define MAX_N 16
	
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
		const uint TRANSITION_POS,NONVAR_COUNT,N,BLOCK_SIZE;
		const bool DRAW_VERTICAL;
		const uint WRAP_SIZE,TUPLE_PRINT_WIDTH; // derived
		
		tuple_config(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,uint INIT_BLOCK_SIZE,bool INIT_DRAW_VERTICAL);
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
		
		filter_store();
	};
	
	// ------------------------------------------------------------ ||
	/*
	  tuple_set: an array of unique N-tuples
	  
	  -       N = 3          len = 2
	  -      |     |         |
	  block: (#,#,#),(#,#,#),(_,_,_),(_,_,_),(_,_,_),...
	*/
	
	class tuple_set_editor;
	
	class tuple_set{
		friend tuple_set_editor;
		
	// Fields ------------------------------------------- |
	protected:
		// Configuration
		tuple_config * const config;
		filter_store * const filter;
		
		tuple_set * const next;
		const char prefix_1,prefix_2;
		const tuple_set * * const supersets; // array expected to have at least config->N elements
		
		// Data
		uint len;
		symb * const block; //................. array expected to have at least config->N * config->BLOCK_SIZE elements
		
		// Draw parameters
		bool is_visible;
		bool are_contents_shown;
		mutable int y;
		
	// Edit methods ------------------------------------- |
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
		
	// Filter methods ----------------------------------- |
		void filter_clear();
		void filter_apply(symb filter_1,symb filter_2,symb filter_3);
		uint filter_results() const;
		
		void filter_nav_next();
		void filter_nav_prev();
		const symb *filter_nav_select() const;
		
	// Draw methods ------------------------------------- |
	protected:
		void shift_y(int delta_y) const;
		
		void adjust_screen_space(uint prev_height) const;
		void clear_screen_space() const;
		void draw_screen_space() const;
		
		void move_to_read_position() const;
		void re_draw() const;
		void re_draw(uint prev_height) const;
		
	public:
		void init_draw(int draw_y) const;
		
		void set_visibility(bool new_visibility);
		void show_contents(bool new_contents_shown);
		
	// OOP object management ---------------------------- |
		tuple_set(tuple_config *init_config,filter_store *init_filter,tuple_set *init_next,char init_prefix_1,char init_prefix_2,const tuple_set * * init_supersets,symb *init_block);
		void set_superset(uint i,const tuple_set *superset);
	};
	
	// declared here, but not defined in tuple_set.cpp
	// whoever triggers tuple_set::on_remove() (in this case, automata.cpp's use of tuple_set_editor.edit()) has the responsibility of defining its value
	extern void (*monad_set_on_remove_callback)(const tuple_set *,symb);
	
	// ------------------------------------------------------------ ||
	class tuple_set_editor{
	private:
		enum read{
			READ_IDEMPOTENT,
			READ_ADD,
			READ_REMOVE,
			READ_SET
		};
		
	// Fields ------------------------------------------- |
		tuple_set *focus;
		read state;
		
		uint pos;
		symb buffer[MAX_N];
		
	// Methods ------------------------------------------ |
		void draw() const;
		void init_read(read new_state);
		
	public:
		tuple_set_editor();
		
		void switch_to(tuple_set *new_focus);
		void edit(int in);
		
		bool is_amid_edit() const;
		const tuple_set *get_superset_current() const;
	};
	
	// ------------------------------------------------------------ ||
	class set: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb block[SET_BLOCK_SIZE];
		
	public:
		static tuple_config config;
		
		set(tuple_set *init_next,char init_prefix_1,char init_prefix_2);
	};
	
	// ------------------------------------------------------------ ||
	class element: public tuple_set{
	private:
		const tuple_set *supersets[1];
		symb block[1];
		
	public:
		static tuple_config config;
		
		element(tuple_set *init_next,char init_prefix_1,char init_prefix_2);
		
		bool is_set() const;
		symb get() const;
	};
	
	// ------------------------------------------------------------ ||
	class product: public tuple_set{
	private:
		tuple_config config;
		filter_store filter;
		
		const tuple_set *supersets[MAX_N];
		symb block[PRODUCT_BLOCK_SIZE];
		
	public:
		product(tuple_set *init_next,uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,char init_prefix_1,char init_prefix_2);
	};
	
#endif