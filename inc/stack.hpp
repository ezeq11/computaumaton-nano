#ifndef STACK_INCLUDED
#define STACK_INCLUDED
	#include "compile_config.hpp"
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#ifdef ARDUINO_NANO_BUILD
		#define STACK_BUFFER_SIZE 64
		#define MAX_STACK_VIEW_WIDTH 32
	#else
		#define STACK_BUFFER_SIZE 128
		#define MAX_STACK_VIEW_WIDTH 64
	#endif
	
	class stack{
		private:
			uint len;
			symb buffer[STACK_BUFFER_SIZE];
			
		public:
			stack();
			
			void clear();
			void push(symb val);
			void pop();
			
			symb top() const;
			
			int draw(int y,int x) const;
			int nodraw(int y) const;
	};
	
#endif