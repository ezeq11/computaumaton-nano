#ifndef TAPE_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#define TAPE_BLOCK_LEN 128
	#define TAPE_DRAW_WINDOW_MAX_WIDTH 64
	
	class tape{
		protected:
			symb state;
			
			uint pos;
			symb block[TAPE_BLOCK_LEN];
			
			int draw_window(int y,int x,uint left_bound,uint right_bound,bool cursor_pointed,bool cursor_stated,bool mark_center) const;
			
		public:
			int nodraw(int y) const;
			
			symb get_state() const;
			symb get_read() const;
	};
	
	// finite uni-directional tape
	class fu_tape: public tape{
		private:
			uint len;
			
		public:
			void init_edit();
			void edit(int in);
			
			void init_simulate(symb init_state);
			bool simulate(symb new_state);
			
			int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const;
	};
	
	// "infinite" bi-directional tape
	class ib_tape: public tape{
		private:
			symb blank_symbol;
			
			void clear();
			
		public:
			enum motion{
				MOTION_NONE,
				MOTION_LEFT,
				MOTION_RIGHT
			};
			
			void init_edit(symb blank);
			void edit(int in);
			
			void init_simulate(symb init_state);
			bool simulate(symb new_state,symb write,motion direction);
			
			int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const;
	};
	
	#define TAPE_INCLUDED
#endif