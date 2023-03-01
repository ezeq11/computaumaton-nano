#include "curses.hpp"
#include "stack.hpp"

stack::stack(screen_space *init_next)
:screen_space(init_next),len(0),is_visible(false){
	
}

void stack::clear(){
	// Data
	len = 0;
	
	// Draw
	if(is_visible){
		move(screen_space::top(),INDENT_X);
		clrtoeol();
		
		addch('[');
		addch(' ');
	}
}

void stack::push(symb val){
	if(len + 1 > STACK_BUFFER_SIZE){
		return;
	}
	
	// Data
	buffer[len] = val;
	++len;
	
	// Draw
	if(len > MAX_STACK_VIEW_WIDTH){
		draw();
		
	}else if(is_visible){
		move(screen_space::top(),INDENT_X + 2 + len - 1);
		addch(ascii(buffer[len - 1]));
	}
}

void stack::pop(){
	if(len == 0){
		return;
	}
	
	// Data
	--len;
	
	// Draw
	if(len + 1 > MAX_STACK_VIEW_WIDTH){
		draw();
		
	}else if(is_visible){
		move(screen_space::top(),INDENT_X + 2 + len);
		delch();
	}
}

symb stack::top() const{
	if(len == 0){
		return SYMBOL_COUNT;
	}
	
	return buffer[len - 1];
}

void stack::demarcate() const{
	screen_space::demarcate(2);
}

void stack::draw() const{
	screen_space::clear();
	
	if(!is_visible){
		return;
	}
	
	move(screen_space::top(),INDENT_X);
	addch('[');
	addch(' ');
	
	uint start = 0;
	
	if(len > MAX_STACK_VIEW_WIDTH){
		addch('.');
		addch('.');
		addch('.');
		addch(' ');
		
		start = len - MAX_STACK_VIEW_WIDTH;
	}
	
	for(uint i = start;i < len;++i){
		addch(ascii(buffer[i]));
	}
}

void stack::set_visible(bool new_visibility){
	if(is_visible == new_visibility){
		return;
	}
	
	is_visible = new_visibility;
	draw();
}