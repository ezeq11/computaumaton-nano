#ifndef PRODUCT_INCLUDED
	#include <cstring>
	
	#include "unsigned.hpp"
	#include "symbol.hpp"
	
	#include "bit_array.hpp"
	#include "component.hpp"
	
	#define PRODUCT_BLOCK_SIZE 128
	
	#define PRODUCT_COMPONENT           \
		component<                      \
			NONVAR_N,N,                 \
			true,true,false,false,      \
			true,true,true,8,BLOCK_SIZE \
		>
	
	template<uint NONVAR_N,uint N,uint BLOCK_SIZE>
	class product: public PRODUCT_COMPONENT{
		private:
			// Tuples
			uint len;
			symb block[BLOCK_SIZE][N];
			
			// Filter
			bool filter_applied,filter_selection_exists;
			uint filter_result_count;
			
			bit_array<BLOCK_SIZE> filter;
			typename bit_array<BLOCK_SIZE>::iter filter_selection;
			
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
			
			bool tuple_j_contains(uint i){
				return block[i][containing_tuple_j] == PRODUCT_COMPONENT::buffer[containing_tuple_j];
			}
			
		public:
			product(char name_1,char name_2)
			:PRODUCT_COMPONENT(name_1,name_2),len(0),filter_applied(false),filter_selection_exists(false),filter_result_count(0),filter(),filter_selection(&filter){
				
			}
			
			// Edit ------------------------------------------------------- ||
			virtual uint size() const{
				return len;
			}
			
			virtual void on_add(){
				// Verify available space
				if(len >= BLOCK_SIZE){
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
				PRODUCT_COMPONENT::buffer[containing_tuple_j] = to_remove;
				
				remove_if(&tuple_j_contains);
			}
			
			// Draw ------------------------------------------------------- ||
			virtual bool horizontal_iter_begin() const{
				// Not implemented
				return false;
			}
			
			virtual bool horizontal_iter_seek() const{
				// Not implemented
				return false;
			}
			
			virtual bool horizontal_iter_is_current() const{
				// Not implemented
				return false;
			}
			
			virtual bool horizontal_iter_is_visible() const{
				// Not implemented
				return false;
			}
			
			virtual symb horizontal_iter_get(uint j) const{
				// Not implemented
				return SYMBOL_COUNT;
			}
			
			virtual bool vertical_is_current(uint i) const{
				return filter_applied && filter_selection_exists && (i == filter_selection.get());
			}
			
			virtual bool vertical_is_visible(uint i) const{
				return (!filter_applied) || (filter_applied && filter.get(i));
			}
			
			virtual symb vertical_get(uint i,uint j) const{
				return block[i][j];
			}
			
			// Specialized ------------------------------------------------ ||
			void filter_clear(){
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
			
			uint filter_results() const{
				return filter_result_count;
			}
			
			void filter_nav_next() const{
				if(!filter_applied || !filter_selection_exists || filter_selection.at_end()){
					return;
				}
				
				filter_selection.seek_next();
			}
			
			void filter_nav_prev() const{
				if(!filter_applied || !filter_selection_exists || filter_selection.at_beginning()){
					return;
				}
				
				filter_selection.seek_prev();
			}
			
			const symb *filter_nav_select() const{
				if(filter_applied && filter_selection_exists){
					return block[filter_selection.get()];
				}else{
					return NULL;
				}
			}
	};
	
	template<uint NONVAR_N,uint N,uint BLOCK_SIZE> uint product<NONVAR_N,N,BLOCK_SIZE>::containing_tuple_j;
	
	#define PRODUCT_INCLUDED
#endif