#ifndef BIT_ARRAY_INCLUDED
	#include <cstdint>
	#include <cstring>
	
	#include "unsigned.hpp"
	
	#define BITS_BYTE_LEN(BIT_LEN) (((BIT_LEN) / 8) + ((BIT_LEN) % 8 > 0))
	
	template<uint BIT_LEN>
	class bit_array{
		private:
			uint8_t bytes[BITS_BYTE_LEN(BIT_LEN)];
			
		public:
			bit_array()
			:bytes{}{
				
			}
			
			void clear(){
				memset(bytes,0,BITS_BYTE_LEN(BIT_LEN) * sizeof(uint8_t));
			}
			
			void invert(){
				for(uint byte = 0;byte < BITS_BYTE_LEN(BIT_LEN);++byte){
					bytes[byte] = ~(bytes[byte]);
				}
			}
			
			void mask_range(uint begin,uint end){ // ANDs the range [begin,end)
				uint begin_byte = begin / 8;
				uint end_byte = end / 8;
				
				for(uint byte = 0;byte < BITS_BYTE_LEN(BIT_LEN);++byte){
					if(byte < begin_byte || byte > end_byte){
						bytes[byte] = 0;
						
					}else if(byte > begin_byte && byte < end_byte){
						// Leave untouched
						
					}else{
						uint8_t mask = 0xff;
						
						if(byte == begin_byte){
							mask &= (0xffu << (begin % 8));
						}
						
						if(byte == end_byte){
							mask &= (0xffu >> (8 - (end % 8)));
						}
						
						bytes[byte] &= mask;
					}
				}
			}
			
			void add(uint i){
				bytes[i / 8] |=  (0x1 << (i % 8));
			}
			
			void remove(uint i){
				bytes[i / 8] &= ~(0x1 << (i % 8));
			}
			
			bool get(uint i) const{
				return (bytes[i / 8] >> (i % 8)) & 0x1;
			}
			
			uint size() const{
				static const uint nibble_size[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
				uint size = 0;
				
				for(uint byte = 0;byte < BITS_BYTE_LEN(BIT_LEN);++byte){
					size += nibble_size[(bytes[byte] >> 0) & 0xf];
					size += nibble_size[(bytes[byte] >> 4) & 0xf];
				}
				
				return size;
			}
			
			class iter{
				private:
					const bit_array<BIT_LEN> * const array;
					mutable uint curr_bit;
					
				public:
					iter(const bit_array<BIT_LEN> * const iterated)
					:array(iterated),curr_bit(0){
						
					}
					
					bool begin() const{
						curr_bit = 0;
						
						if(BIT_LEN == 0){
							return false;
						}else if(array->get(curr_bit)){
							return true;
						}else{
							return seek_next();
						}
					}
					
					bool end() const{
						if(BIT_LEN == 0){
							return false;
						}
						
						curr_bit = BIT_LEN - 1;
						
						if(array->get(curr_bit)){
							return true;
						}else{
							return seek_prev();
						}
					}
					
					bool at_beginning() const{
						if(BIT_LEN == 0){
							return true;
						}
						
						for(uint bit = 0;bit < (curr_bit % 8);++bit){
							if(array->get(((curr_bit / 8) * 8) + bit)){
								return false;
							}
						}
						
						for(uint byte = 0;byte < (curr_bit / 8);++byte){
							if(array->bytes[byte] != 0){
								return false;
							}
						}
						
						return true;
					}
					
					bool at_end() const{
						if(BIT_LEN == 0){
							return true;
						}
						
						for(uint bit = 1;bit < (8 - (curr_bit % 8));++bit){
							if(array->get(curr_bit + bit)){
								return false;
							}
						}
						
						for(uint byte = 1;byte < (BITS_BYTE_LEN(BIT_LEN) - (curr_bit / 8));++byte){
							if(array->bytes[(curr_bit / 8) + byte] != 0){
								return false;
							}
						}
						
						return true;
					}
					
					bool seek_next() const{
						if(curr_bit >= BIT_LEN){
							return false;
						}
						
						++curr_bit;
						
						while(curr_bit < BIT_LEN && !(array->get(curr_bit))){
							if(array->bytes[curr_bit / 8] == 0){
								curr_bit = ((curr_bit / 8) + 1) * 8 + 0;
							}else{
								++curr_bit;
							}
						}
						
						return curr_bit < BIT_LEN;
					}
					
					bool seek_prev() const{
						if(curr_bit == 0){
							return false;
						}
						
						--curr_bit;
						
						while(curr_bit > 0 && !(array->get(curr_bit))){
							if(array->bytes[curr_bit / 8] == 0){
								curr_bit = ((curr_bit / 8) - 1) * 8 + 7;
							}else{
								--curr_bit;
							}
						}
						
						return array->get(curr_bit);
					}
					
					uint get() const{
						if(curr_bit >= BIT_LEN){
							return BIT_LEN;
						}
						
						if(array->get(curr_bit)){
							return curr_bit;
						}else{
							return BIT_LEN;
						}
					}
			};
	};
	
	#define BIT_ARRAY_INCLUDED
#endif