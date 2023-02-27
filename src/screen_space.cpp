#include "curses.hpp"
#include "screen_space.hpp"

void screen_space::shift_y(int delta_y) const{
	y += delta_y;
	
	if(next != NULL){
		next->shift_y(delta_y);
	}
}

screen_space::screen_space(screen_space *init_next)
:next(init_next),y(0),is_visible(true){
	
}

void screen_space::demarcate(int init_height) const{
	if(next == NULL){
		return;
	}
	
	int delta_y = (y + init_height) - next->y;
	next->shift_y(delta_y);
}

void screen_space::resize(int new_height) const{
	if(next == NULL){
		return;
	}
	
	int delta_y = (y + new_height) - next->y;
	
	if(delta_y == 0){
		return;
	}
	
	move(y,0);
	
	if(delta_y > 0){
		for(int l = 0;l < delta_y;++l){
			insertln();
		}
	}else{
		for(int l = 0;l > delta_y;--l){
			deleteln();
		}
	}
	
	next->shift_y(delta_y);
}

void screen_space::clear() const{
	if(next == NULL){
		move(y,0);
		clrtobot();
		
	}else{
		for(int l = 0;l < (next->y - y);++l){
			move(y + l,0);
			clrtoeol();
		}
	}
}