#ifndef STATEFUL_TAPE_INCLUDED
#define STATEFUL_TAPE_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "set.hpp"
	
	#define TAPE_BLOCK_LEN 128
	#define TAPE_DRAW_WINDOW_MAX_WIDTH 64
	
	class stateful_tape{
	private:
		const set * const superset;
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
		
		stateful_tape(const set *init_superset,bool init_bounded);
		
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