#ifndef COMPONENT_INCLUDED
#define COMPONENT_INCLUDED
	#include <cstring>
	
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		#include <curses.h>
	#else
		#include <ncurses.h>
	#endif
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	class set;
	
	class component_interface{
		public:
			virtual void set_superset(uint j,const set *superset) = 0;
			virtual const set *get_superset_current() const = 0;
			
			// Edit ------------------------------------------------------- ||
			virtual void remove_containing(const set *superset,symb to_remove) = 0;
			
			virtual void edit(int in) = 0;
			virtual bool is_amid_edit() const = 0;
			
			// Draw ------------------------------------------------------- ||
			virtual int draw(int y,int x,bool is_current,bool indicate_current_item) const = 0;
			virtual int nodraw(int y) const = 0;
			
			virtual void print_available_commands() const = 0;
	};
	
	#define COMPONENT_TEMPLATE \
		template<uint NONVAR_N,uint N,bool CAN_ADD,bool CAN_REMOVE,bool CAN_SET,bool CAN_CLEAR>
	
	#define COMPONENT_TEMPLATE_ARGS \
		NONVAR_N,N,CAN_ADD,CAN_REMOVE,CAN_SET,CAN_CLEAR
	
	COMPONENT_TEMPLATE
	class component: public component_interface{
		private:
			enum read{
				READ_IDEMPOTENT,
				READ_ADD,
				READ_REMOVE,
				READ_SET
			};
			
			char prefix_1,prefix_2;
			read state;
			
		protected:
			uint pos;
			
			const set *supersets[N];
			symb buffer[N];
			
		private:
			void init_read(read new_state);
			
		public:
			component(char name_1,char name_2);
			
			virtual void set_superset(uint j,const set *superset);
			virtual const set *get_superset_current() const;
			
			// Edit ------------------------------------------------------- ||
			virtual void on_add() = 0;
			virtual void on_remove() = 0;
			virtual void on_set() = 0;
			virtual void on_clear() = 0;
			
			virtual void remove_containing(const set *superset,symb to_remove) = 0;
			
			virtual void edit(int in);
			virtual bool is_amid_edit() const;
			
			// Draw ------------------------------------------------------- ||
			virtual int draw_contents(int y,int x,int *cx,bool indicate_current_item) const = 0;
			virtual void draw_buffer(int *cx) const = 0;
			
			virtual int nodraw_contents(int y) const = 0;
			
			virtual int draw(int y,int x,bool is_current,bool indicate_current_item) const;
			virtual int nodraw(int y) const;
			
			virtual void print_available_commands() const;
	};
	
	#include "set.hpp" // must be included here, since class set{} relies on a fully-declared template component<>
	
	COMPONENT_TEMPLATE
	void component<COMPONENT_TEMPLATE_ARGS>::init_read(read new_state){
		state = new_state;
		pos = 0;
		
		memset(buffer,SYMBOL_COUNT,N * sizeof(symb));
	}
	
	COMPONENT_TEMPLATE
	component<COMPONENT_TEMPLATE_ARGS>::component(char name_1,char name_2)
	:prefix_1(name_1),prefix_2(name_2),state(READ_IDEMPOTENT),pos(0),supersets{}{		
		
	}
	
	COMPONENT_TEMPLATE
	void component<COMPONENT_TEMPLATE_ARGS>::set_superset(uint j,const set *superset){
		supersets[j] = superset;
	}
	
	COMPONENT_TEMPLATE
	const set *component<COMPONENT_TEMPLATE_ARGS>::get_superset_current() const{
		if(pos < N){
			return supersets[pos];
		}else{
			return NULL;
		}
	}
	
	COMPONENT_TEMPLATE
	void component<COMPONENT_TEMPLATE_ARGS>::edit(int in){
		if(state == READ_IDEMPOTENT){
			switch(in){
			case 'u':
			case 'U':
				if(CAN_ADD){
					init_read(READ_ADD);
				}
				
				break;
			case '\\':
				if(CAN_REMOVE){
					init_read(READ_REMOVE);
				}
				
				break;
			case '=':
				if(CAN_SET){
					init_read(READ_SET);
				}
				
				break;
			case '/':
				if(CAN_CLEAR){
					on_clear();
				}
				
				break;
			}
		}else{
			switch(in){
			case '\b':
			case 0x7f:
				if(pos == 0){
					break;
				}
				
				--pos;
				buffer[pos] = SYMBOL_COUNT;
				
				break;
			case '\t':
			case '\n':
				if(pos < NONVAR_N){
					break;
				}
				
				switch(state){
				case READ_IDEMPOTENT:
					break;
				case READ_ADD:
					on_add();
					
					break;
				case READ_REMOVE:
					on_remove();
					
					break;
				case READ_SET:
					on_set();
					
					break;
				}
				
				if(in == '\t' && (state == READ_ADD || state == READ_REMOVE)){
					init_read(state);
				}else{
					state = READ_IDEMPOTENT;
				}
				
				break;
			case '`':
				state = READ_IDEMPOTENT;
				
				break;
			default:
				if(in > (char)in){
					break;
				}
				
				if(pos < N && is_symbol((char)in)){
					symb new_val = symbol((char)in);
					
					if(supersets[pos] == NULL || supersets[pos]->contains(new_val)){
						buffer[pos] = new_val;
						++pos;
					}
				}
				
				break;
			}
		}
	}
	
	COMPONENT_TEMPLATE
	bool component<COMPONENT_TEMPLATE_ARGS>::is_amid_edit() const{
		return state != READ_IDEMPOTENT;
	}
	
	COMPONENT_TEMPLATE
	int component<COMPONENT_TEMPLATE_ARGS>::draw(int y,int x,bool is_current,bool indicate_current_item) const{
		// Prefix -------------------------------
		move(y,x);
		
		addch(is_current && state == READ_IDEMPOTENT ? '>' : ' ');
		addch(' ');
		
		addch(prefix_1);
		addch(prefix_2);
		addch(' ');
		addch('=');
		addch(' ');
		
		x += 7;
		
		// Content ------------------------------
		int cx = x;
		
		if(state != READ_SET){
			y = draw_contents(y,x,&cx,indicate_current_item);
		}
		
		if(state != READ_IDEMPOTENT){
			// Edit buffer --------------------------
			switch(state){
			case READ_IDEMPOTENT:
				break;
			case READ_ADD:
				addch(' ');
				addch('U');
				addch(' ');
				cx += 3;
				
				break;
			case READ_REMOVE:
				addch(' ');
				addch('\\');
				addch(' ');
				cx += 3;
				
				break;
			case READ_SET:
				break;
			}
			
			draw_buffer(&cx);
			
			++y;
			move(y,cx);
			
			if(pos == N){
				addch('<');
			}else{
				addch('^');
			}
		}else{
			++y;
		}
		
		return y + 1;
	}
	
	COMPONENT_TEMPLATE
	int component<COMPONENT_TEMPLATE_ARGS>::nodraw(int y) const{
		return nodraw_contents(y) + 2;
	}
	
	COMPONENT_TEMPLATE
	void component<COMPONENT_TEMPLATE_ARGS>::print_available_commands() const{
		if(is_amid_edit()){
			printw("[`]");
			
			if(pos >= NONVAR_N){
				printw("[tab][enter] --- ");
			}else{
				printw("[   ][     ] --- ");
			}
			
			printw("[backspace][typing] --- | editing");
		}else{
			if(CAN_ADD){
				printw("[U]");
			}
			
			if(CAN_REMOVE){
				printw("[\\]");
			}
			
			if(CAN_SET){
				printw("[=]");
			}
			
			if(CAN_CLEAR){
				printw("[/]");
			}
		}
	}
	
#endif