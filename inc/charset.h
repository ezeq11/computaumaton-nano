#ifndef CHARSET_INCLUDED
	#include "unsigned.h"
	
	/*
	  character set: a range of ASCII characters used to denote mathematical objects
	*/
	
	#define CHARSET_RANGE_BEGIN '!'
	#define CHARSET_RANGE_END '~'
	
	#define CHARSET_SIZE (CHARSET_RANGE_END - CHARSET_RANGE_BEGIN + 1)
	
	uint charset_contains(char ascii);
	
	/*
	  character: an element of the character set, indexed 0 to CHARSET_SIZE - 1
	  
	  Conversions between values that are not a part of the character set will "zero-out";
	  charset() will return 0, ascii() will return CHARSET_RANGE_BEGIN (ascii of index 0)
	*/
	
	uchar charset(char ascii);
	char ascii(uchar charset);
	
	#define CHARSET_INCLUDED
#endif