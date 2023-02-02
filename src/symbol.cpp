#include "symbol.hpp"

#define SYMBOL_RANGE_BEGIN '!'
#define SYMBOL_RANGE_END '~'

bool is_symbol(char ascii){
	return (ascii >= SYMBOL_RANGE_BEGIN) && (ascii <= SYMBOL_RANGE_END);
}

symb symbol(char ascii){
	if(!is_symbol(ascii)){
		return SYMBOL_COUNT;
	}
	
	return ascii - SYMBOL_RANGE_BEGIN;
}

char ascii(symb symbol){
	if(symbol >= SYMBOL_COUNT){
		return ' ';
	}
	
	return symbol + SYMBOL_RANGE_BEGIN;
}