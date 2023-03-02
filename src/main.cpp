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
	
	attrset(A_REVERSE);
	
	move(1,0);
	for(uint x = 0;x < INDENT_X;++x){
		addch(' ');
	}
	
	move(1,INDENT_X);
	switch(current_automaton){
	case 0:
		printw(STRL("M = (S,Q,D,q0,F)           finite-state automaton "));
		break;
	case 1:
		printw(STRL("M = (S,Q,G,D,q0,g0,F)          pushdown automaton "));
		break;
	case 2:
		printw(STRL("M = (S,Q,D,b,q0,F)                 turing machine "));
		break;
	default:
		break;
	}
	
	attrset(A_NORMAL);
	
	automata[current_automaton]->init_draw(3);
}

void re_draw(){
	move(1,INDENT_X + 50);
	
	#ifdef ARDUINO_NANO_BUILD
		printw(automata[current_automaton]->is_interruptible() ? STRL(" ? h l ") : STRL(" ?     "));
	#else
		printw(automata[current_automaton]->is_interruptible() ? STRL(" esc ? h l ") : STRL(" esc ?     "));
	#endif
	
	attrset(A_REVERSE);
	addch(' ');
	attrset(A_NORMAL);
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
	re_draw();
	refresh();
	
	// Update
	in = getch();
	
	if((in == 'h' || in == 'l') && automata[current_automaton]->is_interruptible()){
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