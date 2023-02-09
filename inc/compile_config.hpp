#ifndef COMPILE_CONFIG_INCLUDED
#define COMPILE_CONFIG_INCLUDED
	/*
	  This is the file that determines the build parameters for the executable.
	  
	  Everything has been built utilizing statically allocated arrays, and output is performed with a curses-like library.
	  If ARDUINO_NANO_BUILD is specified, the buffer arrays will be adjusted to smaller sizes that fit into an Arduino Nano.
	  Furthermore, the appropriate main() function adjustments will be made, the appropriate curses-like library will be
	  chosen, and constant strings will be redirected to Flash memory instead of RAM.
	  
	  Uncomment/re-comment the line below to make the desired adjustments.
	*/
	
	// #define ARDUINO_NANO_BUILD
	
#endif