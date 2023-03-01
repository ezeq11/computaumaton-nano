#include "compile_config.hpp"

#ifndef ARDUINO_NANO_BUILD
	#include <cstring>
#endif

#include "curses.hpp"
#include "stateful_tape.hpp"

// ------------------------------------------------------------ ||
#define HALF_WIN_MWIDTH (TAPE_DRAW_WINDOW_MAX_WIDTH / 2)

void stateful_tape::draw_calculate_bounds(uint *left_bound,uint *right_bound,bool *ellipsis_left,bool *ellipsis_right) const{
	*left_bound = 0;
	*right_bound = (is_bounded ? len : TAPE_BLOCK_LEN);
	
	*ellipsis_left = false;
	*ellipsis_right = false;
	
	if(*right_bound - *left_bound > TAPE_DRAW_WINDOW_MAX_WIDTH){
		// Tape display larger than allowable window - adjustment required
		
		if(pos < *left_bound + HALF_WIN_MWIDTH){
			// Against left edge
			*right_bound = *left_bound + TAPE_DRAW_WINDOW_MAX_WIDTH;
			*ellipsis_right = true;
			
		}else if(pos > *right_bound - HALF_WIN_MWIDTH){
			// Against right edge
			*left_bound = *right_bound - TAPE_DRAW_WINDOW_MAX_WIDTH;
			*ellipsis_left = true;
			
		}else{
			// In the middle
			*left_bound = pos - HALF_WIN_MWIDTH;
			*right_bound = pos + HALF_WIN_MWIDTH;
			
			*ellipsis_left = true;
			*ellipsis_right = true;
		}
	}
}

stateful_tape::stateful_tape(screen_space *init_next,const tuple_set *init_superset,bool init_bounded)
:screen_space(init_next),superset(init_superset),is_bounded(init_bounded),current_mode(MODE_NIL){
	
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
	
	current_mode = MODE_EDIT;
	draw();
}

void stateful_tape::edit(int in){
	if(current_mode != MODE_EDIT){
		return;
	}
	
	char char_in;
	symb symb_in;
	
	switch(in){
	case 'h':
		if(pos == 0){
			break;
		}
		
		--pos;
		draw();
		
		break;
	case 'l':
		if(pos >= (is_bounded ? len : TAPE_BLOCK_LEN - 1)){
			break;
		}
		
		++pos;
		draw();
		
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
			
			draw();
		}else{
			block[pos] = blank_symbol;
			
			draw();
		}
		
		break;
	case '|':
		if(is_bounded){
			len = 0;
			pos = 0;
		}else{
			memset(block,blank_symbol,sizeof(block));
		}
		
		draw();
		
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
			
			draw();
		}else{
			block[pos] = symb_in;
			
			draw();
		}
	}
}

// Simulate --------------------------------------------------- ||
void stateful_tape::init_simulate(symb init_state){
	state = init_state;
	pos = (is_bounded ? 0 : TAPE_BLOCK_LEN / 2);
	
	current_mode = MODE_SIMULATE;
	draw();
}

void stateful_tape::simulate(symb new_state,symb write,motion direction){
	if(current_mode != MODE_SIMULATE){
		return;
	}
	
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
	
	draw();
}

bool stateful_tape::at_end() const{
	if(current_mode != MODE_SIMULATE){
		return true;
	}
	
	return is_bounded && pos >= len;
}

symb stateful_tape::get_state() const{
	if(current_mode != MODE_SIMULATE){
		return SYMBOL_COUNT;
	}
	
	return state;
}

symb stateful_tape::get_read() const{
	if(current_mode != MODE_SIMULATE){
		return SYMBOL_COUNT;
	}
	
	return block[pos];
}
// Close ------------------------------------------------------ ||
void stateful_tape::close(){
	current_mode = MODE_NIL;
	draw();
}

// Draw ------------------------------------------------------- ||
void stateful_tape::demarcate() const{
	screen_space::demarcate(4);
}

void stateful_tape::draw() const{
	// Preparation
	screen_space::clear();
	
	if(current_mode == MODE_NIL){
		return;
	}
	
	uint left_bound,right_bound;
	bool ellipsis_left,ellipsis_right;
	
	draw_calculate_bounds(&left_bound,&right_bound,&ellipsis_left,&ellipsis_right);
	
	int y = screen_space::top();
	int x = INDENT_X;
	
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
		
		switch(current_mode){
		case MODE_NIL:
			
			break;
		case MODE_EDIT:
			move(y + 3,cx);
			addch('^');
			
			break;
		case MODE_SIMULATE:
			move(y + 0,cx);
			addch(ascii(state));
			
			move(y + 1,cx);
			addch('v');
			
			break;
		}
	}
}

void stateful_tape::draw_overlay_pipe() const{
	if(current_mode != MODE_SIMULATE){
		return;
	}
	
	uint left_bound,right_bound;
	bool ellipsis_left,ellipsis_right;
	
	draw_calculate_bounds(&left_bound,&right_bound,&ellipsis_left,&ellipsis_right);
	
	int y = screen_space::top();
	int x = INDENT_X;
	
	if(pos >= left_bound && pos <= right_bound){
		move(y + 1,MARKER_X(pos));
		addch('|');
	}
}

void stateful_tape::print_available_commands() const{
	printw(STRL("[|][h][l][backspace][typing]"));
}
