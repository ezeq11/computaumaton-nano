#include "compile_config.hpp"
#include "curses.hpp"

#include "unsigned.hpp"
#include "draw.hpp"
#include "automata.hpp"

// Global automata
fsa finite_state_automaton;
pda pushdown_automaton;
tm turing_machine;

automaton * const automata[3] = {&finite_state_automaton,&pushdown_automaton,&turing_machine};
uint current_automaton = 0;

// Global data
bool illustrate_supersets = true;
int in = 0;

// Program
void loop(){
	// Draw
	/*move(1,INDENT_X);
	switch(current_automaton){
	case 0:
		printw(STRL("M = (S,Q,D,q0,F) ----------- | finite-state automaton "));
		break;
	case 1:
		printw(STRL("M = (S,Q,G,D,q0,g0,F) ------ | pushdown automaton "));
		break;
	case 2:
		printw(STRL("M = (S,Q,D,b,q0,F) --------- | turing machine "));
		break;
	default:
		break;
	}
	
	if(automata[current_automaton]->is_interruptible()){
		move(1,COMMANDS_X);
		printw(STRL("[h][l]"));
	}*/
	
	automata[current_automaton]->draw(3,INDENT_X);
	
	refresh();
	
	// Update
	in = getch();
	
	if(automata[current_automaton]->is_interruptible() && (in == 'h' || in == 'l')){
		current_automaton = (3 + current_automaton - (in == 'h' ? 1 : 0) + (in == 'l' ? 1 : 0)) % 3;
		
		clear();
		automata[current_automaton]->init_draw(3);
	}else if(in == '?'){
		illustrate_supersets = !illustrate_supersets;
	}else{
		automata[current_automaton]->update(in,illustrate_supersets);
	}
}

#ifdef ARDUINO_NANO_BUILD
	// Arduino Nano program --------------------------------------- ||
	void setup(){
		Serial.begin(9600);
		automaton::init();
		
		printw(STRL("\033[?25l"));
		automata[current_automaton]->init_draw(3);
	}
#else
	#include <cstdlib>
	
	// Desktop program -------------------------------------------- ||
	int main(){
		initscr();
		cbreak();
		noecho();
		curs_set(0);
		keypad(stdscr,TRUE);
		
		automaton::init();
		automata[current_automaton]->init_draw(3);
		
		in = 0;
		
		while(in != 0x1b){
			loop();
		}
		
		clear();
		refresh();
		endwin();
		
		printf("fsa: %d bytes\n",sizeof(fsa));
		printf("pda: %d bytes\n",sizeof(pda));
		printf("tm:  %d bytes\n",sizeof(tm));
		
		return EXIT_SUCCESS;
	}
#endif