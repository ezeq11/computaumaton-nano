#include <cstring>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "tapes.hpp"

// ------------------------------------------------------------ ||

#define HALF_WIN_MWIDTH (TAPE_DRAW_WINDOW_MAX_WIDTH / 2)

int tape::draw_window(int y,int x,uint left_bound,uint right_bound,bool cursor_pointed,bool cursor_stated,bool mark_center) const{
	uint center = (right_bound - left_bound) / 2;
	
	// Handle large bounds
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
	
	#define MARKER_X(marker) x + 2 + (ellipsis_left || ellipsis_right ? 4 : 0) + marker - left_bound
	
	// Center marker
	if(mark_center && center >= left_bound && center < right_bound){
		move(y + 3,MARKER_X(center));
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

void tape::set_superset(const set *s){
	superset = s;
}

int tape::nodraw(int y) const{
	return y + 5;
}

symb tape::get_state() const{
	return state;
}

symb tape::get_read() const{
	return block[pos];
}

// ------------------------------------------------------------ ||

void fu_tape::init_edit(){
	pos = 0;
	len = 0;
}

void fu_tape::edit(int in){
	switch(in){
	case KEY_LEFT:
		if(pos == 0){
			break;
		}
		
		--pos;
		break;
	case KEY_RIGHT:
		if(pos >= len){
			break;
		}
		
		++pos;
		break;
	case '\b':
	case 0x7f:
		if(len == 0 || pos == 0){
			break;
		}
		
		memmove(block + pos - 1,block + pos,(len - pos) * sizeof(symb));
		
		--len;
		--pos;
		
		break;
	default:
		if(len >= TAPE_BLOCK_LEN || pos >= TAPE_BLOCK_LEN){
			break;
		}
		
		if(in > (char)in){
			break;
		}
		
		if(!is_symbol((char)in)){
			break;
		}
		
		if(superset != NULL && !(superset->contains(symbol((char)in)))){
			break;
		}
		
		memmove(block + pos + 1,block + pos,(len - pos) * sizeof(symb));
		block[pos] = symbol((char)in);
		
		++len;
		++pos;
		
		break;
	}
}

bool fu_tape::can_simulate() const{
	return len > 0;
}

void fu_tape::init_simulate(symb init_state){
	state = init_state;
	pos = 0;
}

bool fu_tape::simulate(symb new_state){
	state = new_state;
	++pos;
	
	return pos >= len;
}

int fu_tape::draw(int y,int x,bool cursor_pointed,bool cursor_stated) const{
	return draw_window(y,x,0,len,cursor_pointed,cursor_stated,false);
}

void fu_tape::print_available_commands() const{
	printw("left right backspace typing ");
}

// ------------------------------------------------------------ ||

void ib_tape::clear(){
	memset(block,blank_symbol,sizeof(block));
}

void ib_tape::init_edit(symb blank){
	blank_symbol = blank;
	clear();
}

void ib_tape::edit(int in){
	switch(in){
	case KEY_LEFT:
		if(pos == 0){
			break;
		}
		
		--pos;
		break;
	case KEY_RIGHT:
		if(pos + 1 >= TAPE_BLOCK_LEN){
			break;
		}
		
		++pos;
		break;
	case '\b':
	case 0x7f:
		block[pos] = blank_symbol;
		
		break;
	case '|':
		clear();
		
		break;
	default:
		if(in > (char)in){
			break;
		}
		
		if(!is_symbol((char)in)){
			break;
		}
		
		if(superset != NULL && !(superset->contains(symbol((char)in)))){
			break;
		}
		
		block[pos] = symbol((char)in);
		break;
	}
}

bool ib_tape::can_simulate() const{
	return true;
}

void ib_tape::init_simulate(symb init_state){
	state = init_state;
	pos = TAPE_BLOCK_LEN / 2;
}

bool ib_tape::simulate(symb new_state,symb write,motion direction){
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
			if(pos + 1 >= TAPE_BLOCK_LEN){
				break;
			}
			
			++pos;
			break;
	}
	
	return false;
}

int ib_tape::draw(int y,int x,bool cursor_pointed,bool cursor_stated) const{
	return draw_window(y,x,0,TAPE_BLOCK_LEN,cursor_pointed,cursor_stated,true);
}

void ib_tape::print_available_commands() const{
	printw("| left right backspace typing ");
}