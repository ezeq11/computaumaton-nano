#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "stack.hpp"

stack::stack()
:len(0){
	
}

void stack::push(symb val){
	if(len + 1 > STACK_BUFFER_SIZE){
		return;
	}
	
	buffer[len] = val;
	++len;
}

void stack::pop(){
	if(len == 0){
		return;
	}
	
	--len;
}

symb stack::top() const{
	if(len == 0){
		return SYMBOL_COUNT;
	}
	
	return buffer[len - 1];
}

int stack::draw(int y,int x) const{
	uint start = (len > MAX_STACK_VIEW_WIDTH ? len - MAX_STACK_VIEW_WIDTH : 0);
	
	move(y,x);
	
	if(len > MAX_STACK_VIEW_WIDTH){
		addch('.');
		addch('.');
		addch('.');
		addch(' ');
	}
	
	for(uint i = start;i < len;++i){
		addch(ascii(buffer[i]));
	}
	
	return y + 2;
}