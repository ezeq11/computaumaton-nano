#ifndef STACK_INCLUDED
#define STACK_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#define STACK_BUFFER_SIZE 128
	#define MAX_STACK_VIEW_WIDTH 64
	
	class stack{
		private:
			uint len;
			symb buffer[STACK_BUFFER_SIZE];
			
		public:
			stack();
			
			void push(symb val);
			void pop();
			symb top() const;
			
			int draw(int y,int x) const;
	};
	
#endif