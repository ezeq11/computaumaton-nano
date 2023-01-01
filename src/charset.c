#include "charset.h"

uint charset_contains(char ascii){
	return (ascii >= CHARSET_RANGE_BEGIN) && (ascii <= CHARSET_RANGE_END);
}

uchar charset(char ascii){
	if(!charset_contains(ascii)){
		return 0;
	}
	
	return ascii - CHARSET_RANGE_BEGIN;
}

char ascii(uchar charset){
	if(charset >= CHARSET_SIZE){
		return CHARSET_RANGE_BEGIN;
	}
	
	return charset + CHARSET_RANGE_BEGIN;
}