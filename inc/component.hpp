#ifndef COMPONENT_INCLUDED
	#include <cstring>
	
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		#include <curses.h>
	#else
		#include <ncurses.h>
	#endif
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	class component_interface{
		public:
			virtual bool edit(char in) = 0;
			
			virtual int draw(int y,int x,bool is_current,bool draw_filter_current) const = 0;
			virtual int nodraw(int y) const = 0;
	};
	
	class set;
	
	template<
		uint NONVAR_N,uint N,
		bool CAN_ADD,bool CAN_REMOVE,bool CAN_SET,bool CAN_CLEAR,
		bool DRAW_VERTICAL,bool PARANTHESIZE,bool BRACKET,uint WRAP_SIZE,uint MAX_SIZE
	>
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
			uint pos;
			
		protected:
			const set *parents[N];
			symb buffer[N];
			
		private:
			void init_read(read new_state){
				state = new_state;
				pos = 0;
				
				memset(buffer,SYMBOL_COUNT,N * sizeof(symb));
			}
			
			uint append_connective(uint j,bool is_visible) const{
				if(j + 1 < N && j < NONVAR_N){
					addch(is_visible ? ',' : ' ');
					return 1;
				}
				
				return 0;
			}
			
		public:
			component(char name_1,char name_2)
			:prefix_1(name_1),prefix_2(name_2),state(READ_IDEMPOTENT),pos(0){
				
			}
			
			// Edit ------------------------------------------------------- ||
			virtual uint size() const = 0;
			
			virtual void on_add() = 0;
			virtual void on_remove() = 0;
			virtual void on_set() = 0;
			virtual void on_clear() = 0;
			
			virtual bool edit(char in){
				if(state == READ_IDEMPOTENT){
					switch(in){
					case 'u':
					case 'U':
						if(CAN_ADD && size() < MAX_SIZE){
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
						
						return true;
					default:
						return true;
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
							return true;
						}
						
						break;
					case '`':
						state = READ_IDEMPOTENT;
						
						return true;
					default:
						if(in > (char)in){
							break;
						}
						
						if(pos < N && is_symbol((char)in)){
							buffer[pos] = symbol((char)in);
							++pos;
						}
						
						break;
					}
				}
				
				return false;
			}
			
			// Draw ------------------------------------------------------- ||
			virtual bool horizontal_iter_begin() const = 0;
			virtual bool horizontal_iter_seek() const = 0;
			
			virtual bool horizontal_iter_is_current() const = 0;
			virtual bool horizontal_iter_is_visible() const = 0;
			virtual symb horizontal_iter_get(uint j) const = 0;
			
			virtual bool vertical_is_current(uint i) const = 0;
			virtual bool vertical_is_visible(uint i) const = 0;
			virtual symb vertical_get(uint i,uint j) const = 0;
			
			virtual int draw(int y,int x,bool is_current,bool draw_filter_current) const{
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
					uint len = size();
					
					if(DRAW_VERTICAL){
						// Vertical draw ------------------------
						uint height = len > WRAP_SIZE ? WRAP_SIZE : len;
						
						if(BRACKET){
							addch('{');
						}else{
							height = (height == 0) ? 1 : height;
						}
						
						for(uint r = 0;r < height;++r){
							uint width = (len / height) + (r < (len % height));
							
							if(BRACKET){
								move(y + 1 + r,x + 2);
							}else{
								move(y + r,x);
							}
							
							for(uint c = 0;c < width;++c){
								uint i = c * height + r;
								bool is_visible = vertical_is_visible(i);
								
								if(is_visible){
									if(PARANTHESIZE){
										addch('(');
									}
									
									for(uint j = 0;j < N;++j){
										addch(ascii(vertical_get(i,j)));
										append_connective(j,true);
									}
									
									if(PARANTHESIZE){
										addch(')');
									}
								}else{
									if(PARANTHESIZE){
										addch(' ');
									}
									
									for(uint j = 0;j < N;++j){
										addch(' ');
										append_connective(j,false);
									}
									
									if(PARANTHESIZE){
										addch(' ');
									}
								}
								
								if(is_visible && i + 1 < len){
									addch(',');
								}else{
									addch(' ');
								}
								
								if(draw_filter_current && vertical_is_current(i)){
									addch('<');
								}else{
									addch(' ');
								}
								
								if(c + 1 < width){
									addch(' ');
								}
							}
						}
						
						if(BRACKET){
							move(y + 1 + height + 1,x);
							addch('}');
							
							y = y + 1 + height + 1;
							++cx;
						}else{
							y = y + height - 1;
						}
					}else{
						// Horizontal draw ----------------------
						bool indented = len > WRAP_SIZE;
						
						if(BRACKET){
							addch('{');
							addch(' ');
							
							cx += 2;
							
							if(indented){
								++y;
								move(y,x + 2);
								
								cx = x + 2;
							}
						}
						
						bool draw_more = horizontal_iter_begin();
						uint column = 0;
						
						while(draw_more){
							bool is_visible = horizontal_iter_is_visible();
							
							if(PARANTHESIZE){
								addch(is_visible ? '(' : ' ');
								++cx;
							}
							
							for(uint j = 0;j < N;++j){
								addch(is_visible ? ascii(horizontal_iter_get(j)) : ' ');
								++cx;
								
								cx += append_connective(j,is_visible);
							}
							
							if(PARANTHESIZE){
								addch(is_visible ? ')' : ' ');
								++cx;
							}
							
							draw_more = horizontal_iter_seek();
							++column;
							
							if(draw_filter_current && horizontal_iter_is_current()){
								addch('<');
							}else if(is_visible && draw_more){
								addch(',');
							}else{
								addch(' ');
							}
							
							++cx;
							
							if(draw_more){
								if(column < WRAP_SIZE){
									addch(' ');
									++cx;
								}else{
									++y;
									move(y,x + 2);
									
									column = 0;
									cx = x + 2;
								}
							}
						}
						
						if(BRACKET){
							if(indented){
								y += 2;
								move(y,x);
								
								addch('}');
								cx = x + 1;
							}else{
								addch('}');
								++cx;
							}
						}
					}
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
					
					if(BRACKET){
						addch('{');
						addch(' ');
						
						cx += 2;
					}
					
					if(PARANTHESIZE){
						addch('(');
						++cx;
					}
					
					for(uint j = 0;j < N;++j){
						if(j < pos){
							addch(ascii(buffer[j]));
							++cx;
							
							cx += append_connective(j,true);
						}else{
							addch(ascii(buffer[j]));
							append_connective(j,true);
						}
					}
					
					if(PARANTHESIZE){
						addch(')');
						
						if(pos == N){
							++cx;
						}
					}
					
					if(BRACKET){
						addch(' ');
						addch('}');
					}
					
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
			
			virtual int nodraw(int y) const{
				uint len = size();
				
				if(DRAW_VERTICAL){
					// Vertical no-draw ---------------------
					uint height = len > WRAP_SIZE ? WRAP_SIZE : len;
					
					if(BRACKET){
						return y + 1 + height + 1 + 2;
					}else{
						return y + (height == 0 ? 1 : height) + 1;
					}
				}else{
					// Horizontal no-draw -------------------
					if(len > WRAP_SIZE){
						return y + (uint)BRACKET + (len / WRAP_SIZE) + (uint)(len % WRAP_SIZE > 0) + (uint)BRACKET + 2;
					}else{
						return y + 2;
					}
				}
			}
	};
	
	#define COMPONENT_INCLUDED
#endif