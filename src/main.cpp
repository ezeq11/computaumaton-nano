#include "compile_config.hpp"
#include "curses.hpp"

#include "unsigned.hpp"
#include "automata.hpp"

fsa finite_state_automaton;
pda pushdown_automaton;
tm turing_machine;

automaton * const automata[3] = {&finite_state_automaton,&pushdown_automaton,&turing_machine};
uint current_automaton = 0;

void init_draw(){
	clear();
	
	move(1,INDENT_X - 1);
	switch(current_automaton){
	case 0:
		attron(A_REVERSE);
		printw(STRL(" M = (S,Q,D,q0,F) "));
		
		attroff(A_REVERSE);
		printw(STRL("----------- | finite-state automaton "));
		
		break;
	case 1:
		attron(A_REVERSE);
		printw(STRL(" M = (S,Q,G,D,q0,g0,F) "));
		
		attroff(A_REVERSE);
		printw(STRL("------ | pushdown automaton "));
		
		break;
	case 2:
		attron(A_REVERSE);
		printw(STRL(" M = (S,Q,D,b,q0,F) "));
		
		attroff(A_REVERSE);
		printw(STRL("--------- | turing machine "));
		
		break;
	default:
		break;
	}
	
	automata[current_automaton]->init_draw(3);
}

// Superset illustration
bool superset_illustration = true;

void set_superset_illustration(bool new_superset_illustration){
	superset_illustration = new_superset_illustration;
	automata[current_automaton]->illustrate_supersets(superset_illustration);
}

void toggle_superset_illustration(){
	superset_illustration = !superset_illustration;
	automata[current_automaton]->illustrate_supersets(superset_illustration);
}

void apply_superset_illustration(){
	automata[current_automaton]->illustrate_supersets(superset_illustration);
}

// Program
int in = 0;

void loop(){
	// Draw
	/*
	if(automata[current_automaton]->is_interruptible()){
		move(1,COMMANDS_X);
		printw(STRL("[h][l]"));
	}*/
	
	refresh();
	
	// Update
	in = getch();
	
	if(automata[current_automaton]->is_interruptible() && (in == 'h' || in == 'l')){
		current_automaton = (3 + current_automaton - (in == 'h' ? 1 : 0) + (in == 'l' ? 1 : 0)) % 3;
		
		init_draw();
		apply_superset_illustration();
	}else if(in == '?'){
		toggle_superset_illustration();
	}else{
		automata[current_automaton]->update(in,superset_illustration);
	}
}

#ifdef ARDUINO_NANO_BUILD
	// Arduino Nano program --------------------------------------- ||
	void setup(){
		Serial.begin(9600);
		printw(STRL("\033[?25l"));
		
		automaton::init();
		
		init_draw();
		set_superset_illustration(true);
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
		
		init_draw();
		set_superset_illustration(true);
		
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