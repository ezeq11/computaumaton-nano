#ifndef CURSES_INCLUDED
#define CURSES_INCLUDED
	#include "compile_config.hpp"
	
	/*
	  This file selects the appropriate curses variant for the approriate build platform and defines/guarantees
	  a common set of IO functions on top of them.
	  
	  Arduino Nano: custom implementation using ANSI escape sequences through Serial
	  Windows: PDCurses
	  Linux: ncurses
	  
	  Routines guaranteed to be available:
	    Output ----
	    - clear
	    - refresh
	    
	    - clrtoeol
	    - clrtobot
	    - insertln
	    - deleteln
	    
	    - move
	    - addch
	    - printw
	    
	    Input -----
	    - getch
	    - set_timeout (custom stateful wrapper for timeout)
	*/
	
	#ifdef ARDUINO_NANO_BUILD
		#define STRL(s) F(s)
	#else
		#define STRL(s) s
	#endif
	
	#ifdef ARDUINO_NANO_BUILD
		// Output ----
		void clear();
		void refresh();
		
		void clrtoeol();
		void clrtobot();
		void insertln();
		void deleteln();
		
		void move(int y,int x);
		void addch(char c);
		void printw(const __FlashStringHelper* str);
		
		// Input -----
		int getch();
		
	#else
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
			#include <curses.h>
		#else
			#include <ncurses.h>
		#endif
	#endif
	
	void set_timeout(int delay);
	
#endif