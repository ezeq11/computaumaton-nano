#ifndef SYMBOL_INCLUDED
	#define SYMBOL_COUNT 94
	typedef unsigned char symb;
	
	/*
	  symbols: a set of ASCII characters used to denote mathematical objects, indexed 0 to SYMBOL_COUNT - 1
	  
	  Conversions between values that are not a part of the symbol set will "null-out";
	  symbol() will return SYMBOL_COUNT, ascii() will return ' '.
	*/
	
	bool is_symbol(char ascii);
	symb    symbol(char ascii);
	
	char ascii(symb symbol);
	
	#define SYMBOL_INCLUDED
#endif