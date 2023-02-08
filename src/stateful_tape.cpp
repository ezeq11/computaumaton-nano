#include <cstring>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "stateful_tape.hpp"

// ------------------------------------------------------------ ||
stateful_tape::stateful_tape(const set *init_superset,bool init_bounded)
:superset(init_superset),is_bounded(init_bounded){
	
}

// Edit ------------------------------------------------------- ||
void stateful_tape::init_edit(symb init_blank){
	if(is_bounded){
		len = 0;
		pos = 0;
		
	}else{
		blank_symbol = init_blank;
		memset(block,blank_symbol,sizeof(block));
		
		pos = TAPE_BLOCK_LEN / 2;
	}
}

void stateful_tape::edit(int in){
	char char_in;
	symb symb_in;
	
	switch(in){
	case KEY_LEFT:
		if(pos == 0){
			break;
		}
		
		--pos;
		break;
	case KEY_RIGHT:
		if(pos >= (is_bounded ? len : TAPE_BLOCK_LEN - 1)){
			break;
		}
		
		++pos;
		break;
	case '\b':
	case 0x7b:
		if(is_bounded){
			if(len == 0 || pos == 0){
				break;
			}
			
			memmove(block + pos - 1,block + pos,(len - pos) * sizeof(symb));
			
			--len;
			--pos;
		}else{
			block[pos] = blank_symbol;
		}
		
		break;
	case '|':
		if(is_bounded){
			break;
		}else{
			memset(block,blank_symbol,sizeof(block));
		}
		
		break;
	default:
		// Check conditions
		char_in = (char)in;
		
		if(in > char_in){
			break;
		}
		
		if(!is_symbol(char_in)){
			break;
		}
		
		symb_in = symbol(char_in);
		
		if(superset != NULL && !superset->contains(symb_in)){
			break;
		}
		
		// Insert
		if(is_bounded){
			if(len >= TAPE_BLOCK_LEN){
				break;
			}
			
			memmove(block + pos + 1,block + pos,(len - pos) * sizeof(symb));
			block[pos] = symb_in;
			
			++len;
			++pos;
		}else{
			block[pos] = symb_in;
		}
	}
}

// Simulate --------------------------------------------------- ||
void stateful_tape::init_simulate(symb init_state){
	state = init_state;
	pos = (is_bounded ? 0 : TAPE_BLOCK_LEN / 2);
}

void stateful_tape::simulate(symb new_state,symb write,motion direction){
	state = new_state;
	block[pos] = write;
	
	switch(direction){
	case MOTION_NONE:
		break;
	case MOTION_LEFT:
		if(pos == 0){
			break;
		}
		
		--pos;
		break;
	case MOTION_RIGHT:
		if(pos >= (is_bounded ? len : TAPE_BLOCK_LEN - 1)){
			break;
		}
		
		++pos;
		break;
	}
}

bool stateful_tape::at_end() const{
	return is_bounded && pos >= len;
}

symb stateful_tape::get_state() const{
	return state;
}

symb stateful_tape::get_read() const{
	return block[pos];
}

// Draw ------------------------------------------------------- ||
#define HALF_WIN_MWIDTH (TAPE_DRAW_WINDOW_MAX_WIDTH / 2)

int stateful_tape::draw(int y,int x,bool cursor_pointed,bool cursor_stated) const{
	uint left_bound = 0;
	uint right_bound = (is_bounded ? len : TAPE_BLOCK_LEN);
	
	// Bound adjustment
	bool ellipsis_left = false;
	bool ellipsis_right = false;
	
	if(right_bound - left_bound > TAPE_DRAW_WINDOW_MAX_WIDTH){
		// Tape display larger than allowable window - adjustment required
		
		if(pos < left_bound + HALF_WIN_MWIDTH){
			// Against left edge
			right_bound = left_bound + TAPE_DRAW_WINDOW_MAX_WIDTH;
			ellipsis_right = true;
			
		}else if(pos > right_bound - HALF_WIN_MWIDTH){
			// Against right edge
			left_bound = right_bound - TAPE_DRAW_WINDOW_MAX_WIDTH;
			ellipsis_left = true;
			
		}else{
			// In the middle
			left_bound = pos - HALF_WIN_MWIDTH;
			right_bound = pos + HALF_WIN_MWIDTH;
			
			ellipsis_left = true;
			ellipsis_right = true;
		}
	}
	
	// Header
	move(y + 2,x);
	
	addch(':');
	addch(' ');
	
	// Left ellipsis
	if(ellipsis_left || ellipsis_right){
		if(ellipsis_left){
			addch('.');
			addch('.');
			addch('.');
			addch(' ');
		}else{
			addch(' ');
			addch(' ');
			addch(' ');
			addch(' ');
		}
	}
	
	// Tape contents
	for(uint i = left_bound;i < right_bound;++i){
		addch(ascii(block[i]));
	}
	
	// Right ellipsis
	if(ellipsis_left || ellipsis_right){
		if(ellipsis_right){
			addch(' ');
			addch('.');
			addch('.');
			addch('.');
		}else{
			addch(' ');
			addch(' ');
			addch(' ');
			addch(' ');
		}
	}
	
	#define MARKER_X(marker) x + 2 + (ellipsis_left || ellipsis_right ? 4 : 0) + (marker) - left_bound
	
	// Center marker
	if(!is_bounded){
		move(y + 3,MARKER_X(TAPE_BLOCK_LEN / 2));
		addch('*');
	}
	
	// Position cursor
	if(pos >= left_bound && pos <= right_bound){
		int cx = MARKER_X(pos);
		
		if(cursor_stated){
			move(y + 0,cx);
			addch(ascii(state));
			
			move(y + 1,cx);
			addch(cursor_pointed ? 'v' : '|');
			
		}else if(cursor_pointed){
			move(y + 3,cx);
			addch('^');
		}
	}
	
	return y + 5;
}

int stateful_tape::nodraw(int y) const{
	return y + 5;
}

void stateful_tape::print_available_commands() const{
	if(is_bounded){
		printw("left right backspace typing ");
	}else{
		printw("| left right backspace typing ");
	}
}

// ------------------------------------------------------------ ||