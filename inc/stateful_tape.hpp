#ifndef STATEFUL_TAPE_INCLUDED
#define STATEFUL_TAPE_INCLUDED
	#include "compile_config.hpp"
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "screen_space.hpp"
	#include "tuple_set.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define TAPE_BLOCK_LEN 64
		#define TAPE_DRAW_WINDOW_MAX_WIDTH 32
	#else
		#define TAPE_BLOCK_LEN 128
		#define TAPE_DRAW_WINDOW_MAX_WIDTH 64
	#endif
	
	class stateful_tape: public screen_space{
	public:
		enum mode{
			MODE_NIL,
			MODE_EDIT,
			MODE_SIMULATE
		};
		
	private:
		const tuple_set * const superset;
		const bool is_bounded;
		
		mode current_mode;
		
		symb state;
		symb blank_symbol;
		
		uint len;
		uint pos;
		symb block[TAPE_BLOCK_LEN];
		
		void draw_calculate_bounds(uint *left_bound,uint *right_bound,bool *ellipsis_left,bool *ellipsis_right) const;
		
	public:
		enum motion{
			MOTION_NONE,
			MOTION_LEFT,
			MOTION_RIGHT
		};
		
		stateful_tape(screen_space *init_next,const tuple_set *init_superset,bool init_bounded);
		
		// Edit ------------------------------------------------------- ||
		void init_edit(symb init_blank);
		void edit(int in);
		
		// Simulate --------------------------------------------------- ||
		void init_simulate(symb init_state);
		void simulate(symb new_state,symb write,motion direction);
		
		bool at_end() const;
		symb get_state() const;
		symb get_read() const;
		
		// Close ------------------------------------------------------ ||
		void close();
		
		// Draw ------------------------------------------------------- ||
		void demarcate() const;
		void draw() const;
		void draw_overlay_pipe() const;
		
		void print_available_commands() const;
	};
	
#endif