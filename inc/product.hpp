#ifndef PRODUCT_INCLUDED
	#include <cstring>
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "bit_array.hpp"
	#include "component.hpp"
	
	#define PRODUCT_BLOCK_SIZE 128
	
	class product_interface{
		public:
			virtual void filter_clear() = 0;
			// filter_apply() method expected to be defined with template parameters
			
			virtual uint filter_results() const = 0;
			virtual void filter_nav_next() const = 0;
			virtual void filter_nav_prev() const = 0;
			
			virtual const symb *filter_nav_select() const = 0;
	};
	
	#define PRODUCT_COMPONENT \
		component<NONVAR_N,N,true,true,false,false>
	
	template<uint NONVAR_N,uint N>
	class product: public product_interface, public PRODUCT_COMPONENT{
		private:
			// Tuples
			uint len;
			symb block[PRODUCT_BLOCK_SIZE][N];
			
			// Filter
			bool filter_applied,filter_selection_exists;
			uint filter_result_count;
			
			bit_array<PRODUCT_BLOCK_SIZE> filter;
			typename bit_array<PRODUCT_BLOCK_SIZE>::iter filter_selection;
			
			// Tuple removal infrastructure
			void remove_if(bool (product::*remove_condition)(uint)){
				// O(n) algorithm: copy only if not deleted
				uint dest_i = 0;
				
				for(uint src_i = 0;src_i < len;++src_i){
					if((this->*remove_condition)(src_i)){
						// Skip if to be removed
						continue;
					}
					
					// Otherwise, copy
					if(dest_i != src_i){
						memcpy(block + dest_i,block + src_i,N * sizeof(symb));
					}
					
					++dest_i;
				}
				
				// Done!
				len = dest_i;
			}
			
			bool tuple_equals_buffer(uint i){
				return memcmp(block[i],PRODUCT_COMPONENT::buffer,N * sizeof(symb)) == 0;
			}
			
			static uint containing_tuple_j;
			static symb containing_tuple_to_remove;
			
			bool tuple_j_contains(uint i){
				return block[i][containing_tuple_j] == containing_tuple_to_remove;
			}
			
		public:
			product(char name_1,char name_2)
			:PRODUCT_COMPONENT(name_1,name_2),len(0),filter_applied(false),filter_selection_exists(false),filter_result_count(0),filter(),filter_selection(&filter){
				
			}
			
			// Edit ------------------------------------------------------- ||
			virtual void on_add(){
				// Verify available space
				if(len >= PRODUCT_BLOCK_SIZE){
					return;
				}
				
				// Verify uniqueness
				for(uint i = 0;i < len;++i){
					if(memcmp(block[i],PRODUCT_COMPONENT::buffer,N * sizeof(symb)) == 0){
						return;
					}
				}
				
				// Locate insertion position according to dictionary order
				uint insert_i = 0;
				uint comparison_j = 0;
				
				while(comparison_j < N){
					while(insert_i < len && block[insert_i][comparison_j] < PRODUCT_COMPONENT::buffer[comparison_j]){
						++insert_i;
					}
					
					++comparison_j;
				}
				
				// Insert
				memmove(block + insert_i + 1,block + insert_i,(len - insert_i) * N * sizeof(symb));
				memcpy(block + insert_i,PRODUCT_COMPONENT::buffer,N * sizeof(symb));
				
				++len;
			}
			
			virtual void on_remove(){
				remove_if(&tuple_equals_buffer);
			}
			
			virtual void on_set(){
				// Not implemented
			}
			
			virtual void on_clear(){
				// Not implemeneted
			}
			
			virtual void remove_containing(uint j,symb to_remove){
				containing_tuple_j = j;
				containing_tuple_to_remove = to_remove;
				
				remove_if(&tuple_j_contains);
			}
			
			// Draw ------------------------------------------------------- ||
			#define PRODUCT_WRAP_SIZE 8
			
			virtual int draw_contents(int y,int x,int *cx,bool indicate_current_item) const{
				uint height = len > PRODUCT_WRAP_SIZE ? PRODUCT_WRAP_SIZE : len;
				
				move(y,x);
				addch('{');
				
				for(uint r = 0;r < height;++r){
					uint width = (len / height) + (r < (len % height));
					
					move(y + 1 + r,x + 2);
					
					for(uint c = 0;c < width;++c){
						uint i = c * height + r;
						bool is_visible = (!filter_applied) || (filter_applied && filter.get(i));
						
						if(is_visible){
							addch('(');
							
							for(uint j = 0;j < N;++j){
								addch(ascii(block[i][j]));
								
								if(j + 1 < N && j < NONVAR_N){
									addch(',');
								}
							}
							
							addch(')');
						}else{
							addch(' ');
							
							for(uint j = 0;j < N;++j){
								addch(' ');
								
								if(j + 1 < N && j < NONVAR_N){
									addch(' ');
								}
							}
							
							addch(' ');
						}
						
						if(is_visible && i + 1 < len){
							addch(',');
						}else{
							addch(' ');
						}
						
						if(indicate_current_item && filter_applied && filter_selection_exists && i == filter_selection.get()){
							addch('<');
						}else{
							addch(' ');
						}
						
						if(c + 1 < width){
							addch(' ');
						}
					}
				}
				
				move(y + 1 + height + 1,x);
				addch('}');
				
				*cx = x + 1;
				return y + 1 + height + 1;
			}
			
			virtual void draw_buffer(int *cx) const{
				addch('{');
				addch(' ');
				addch('(');
				*cx += 3;
				
				for(uint j = 0;j < N;++j){
					addch(ascii(PRODUCT_COMPONENT::buffer[j]));
					*cx += (j < PRODUCT_COMPONENT::pos ? 1 : 0);
					
					if(j + 1 < N && j < NONVAR_N){
						addch(',');
						*cx += (j < PRODUCT_COMPONENT::pos ? 1 : 0);
					}
				}
				
				if(PRODUCT_COMPONENT::pos == N){
					*cx += 1;
				}
				
				addch(')');
				addch(' ');
				addch('}');
			}
			
			virtual int nodraw_contents(int y) const{
				return y + 1 + (len > PRODUCT_WRAP_SIZE ? PRODUCT_WRAP_SIZE : len) + 1;
			}
			
			// Specialized ------------------------------------------------ ||
			virtual void filter_clear(){
				filter_applied = false;
				filter_selection_exists = false;
				filter_result_count = 0;
			}
			
			void filter_apply(const symb filter_vals[N]){
				filter.clear();
				
				for(uint j = 0;j < N;++j){
					if(filter_vals[j] == SYMBOL_COUNT){
						continue;
					}
					
					for(uint i = 0;i < len;++i){
						if(block[i][j] != filter_vals[j]){
							filter.add(i);
						}
					}
				}
				
				filter.invert();
				filter.mask_range(0,len);
				
				filter_applied = true;
				filter_selection_exists = filter_selection.begin();
				filter_result_count = filter.size();
			}
			
			virtual uint filter_results() const{
				return filter_result_count;
			}
			
			virtual void filter_nav_next() const{
				if(!filter_applied || !filter_selection_exists || filter_selection.at_end()){
					return;
				}
				
				filter_selection.seek_next();
			}
			
			virtual void filter_nav_prev() const{
				if(!filter_applied || !filter_selection_exists || filter_selection.at_beginning()){
					return;
				}
				
				filter_selection.seek_prev();
			}
			
			virtual const symb *filter_nav_select() const{
				if(filter_applied && filter_selection_exists){
					return block[filter_selection.get()];
				}else{
					return NULL;
				}
			}
	};
	
	template<uint NONVAR_N,uint N> uint product<NONVAR_N,N>::containing_tuple_j;
	template<uint NONVAR_N,uint N> symb product<NONVAR_N,N>::containing_tuple_to_remove;
	
	#define PRODUCT_INCLUDED
#endif