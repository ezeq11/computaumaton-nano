#ifndef STACK_INCLUDED
#define STACK_INCLUDED
	#include "compile_config.hpp"
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "screen_space.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define STACK_BUFFER_SIZE 64
		#define MAX_STACK_VIEW_WIDTH 32
	#else
		#define STACK_BUFFER_SIZE 128
		#define MAX_STACK_VIEW_WIDTH 64
	#endif
	
	class stack: public screen_space{
		private:
			uint len;
			symb buffer[STACK_BUFFER_SIZE];
			
			bool is_visible;
			
		public:
			stack(screen_space *init_next);
			
			void clear();
			void push(symb val);
			void pop();
			
			symb top() const;
			
			void demarcate() const;
			void draw() const;
			void set_visible(bool new_visibility);
	};
	
#endif