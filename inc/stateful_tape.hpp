#ifndef STATEFUL_TAPE_INCLUDED
#define STATEFUL_TAPE_INCLUDED
	#include "compile_config.hpp"
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "tuple_set.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define TAPE_BLOCK_LEN 64
		#define TAPE_DRAW_WINDOW_MAX_WIDTH 32
	#else
		#define TAPE_BLOCK_LEN 128
		#define TAPE_DRAW_WINDOW_MAX_WIDTH 64
	#endif
	
	class stateful_tape{
	private:
		const tuple_set * const superset;
		const bool is_bounded;
		
		symb state;
		symb blank_symbol;
		
		uint len;
		uint pos;
		symb block[TAPE_BLOCK_LEN];
		
	public:
		enum motion{
			MOTION_NONE,
			MOTION_LEFT,
			MOTION_RIGHT
		};
		
		stateful_tape(const tuple_set *init_superset,bool init_bounded);
		
		// Edit ------------------------------------------------------- ||
		void init_edit(symb init_blank);
		void edit(int in);
		
		// Simulate --------------------------------------------------- ||
		void init_simulate(symb init_state);
		void simulate(symb new_state,symb write,motion direction);
		
		bool at_end() const;
		symb get_state() const;
		symb get_read() const;
		
		// Draw ------------------------------------------------------- ||
		int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const;
		int nodraw(int y) const;
		
		void print_available_commands() const;
	};
	
#endif