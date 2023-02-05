#ifndef TAPE_INCLUDED
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "set.hpp"
	
	#define TAPE_BLOCK_LEN 128
	#define TAPE_DRAW_WINDOW_MAX_WIDTH 64
	
	class tape{
		protected:
			symb state;
			
			const set *superset;
			uint pos;
			symb block[TAPE_BLOCK_LEN];
			
			int draw_window(int y,int x,uint left_bound,uint right_bound,bool cursor_pointed,bool cursor_stated,bool mark_center) const;
			
		public:
			void set_superset(const set *s);
			
			symb get_state() const;
			symb get_read() const;
			
			virtual void init_edit(symb blank) = 0;
			virtual void edit(int in) = 0;
			
			virtual bool can_simulate() const = 0;
			virtual void init_simulate(symb init_state) = 0;
			// simulate() method expected to be defined by children with varying parameters
			
			virtual int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const = 0;
			int nodraw(int y) const;
			
			virtual void print_available_commands() const = 0;
	};
	
	// finite uni-directional tape
	class fu_tape: public tape{
		private:
			uint len;
			
		public:
			virtual void init_edit(symb blank);
			virtual void edit(int in);
			
			virtual bool can_simulate() const;
			virtual void init_simulate(symb init_state);
			bool simulate(symb new_state);
			
			virtual int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const;
			virtual void print_available_commands() const;
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
			
			virtual void init_edit(symb blank);
			virtual void edit(int in);
			
			virtual bool can_simulate() const;
			virtual void init_simulate(symb init_state);
			bool simulate(symb new_state,symb write,motion direction);
			
			virtual int draw(int y,int x,bool cursor_pointed,bool cursor_stated) const;
			virtual void print_available_commands() const;
	};
	
	#define TAPE_INCLUDED
#endif