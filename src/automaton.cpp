#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

#include "automaton.hpp"

fsa::fsa()
:current_focus(FOCUS_S),interfaces{&S,&Q,&q0,&D,&F},S(' ','S',NULL),Q(' ','Q',NULL),q0('q','0'),D(' ','D'),F(' ','F',NULL){
	
}

void fsa::update(int in){
	if(!(interfaces[current_focus]->edit((char)in))){
		return;
	}
	
	switch(in){
	case KEY_UP:
	case KEY_DOWN:
		current_focus = (focus)(((int)FOCUS_COUNT + (int)current_focus - (in == KEY_UP) + (in == KEY_DOWN)) % (int)FOCUS_COUNT);
		
		break;
	}
}

int fsa::draw(int y,int x) const{
	for(uint i = 0;i < FOCUS_COUNT;++i){
		y = interfaces[i]->draw(y,x,i == (uint)current_focus);
	}
	
	return y;
}