#ifndef SCREEN_SPACE_INCLUDED
#define SCREEN_SPACE_INCLUDED
	/*
	  screen_space: a vertical stack of screen partitions
	  
	  example: S1->S2->S3->NULL
	  - 
	  -  _top of screen_________________
	  - |-------------------------------|- S1.y
	  - | area reserved for S1 contents |
	  - |                               |
	  - |-------------------------------|- S2.y
	  - | area reserved for S2 contents |
	  - |-------------------------------|- S3.y
	  - | area reserved for S3 contents |
	  - |                               |
	  - |                               |
	  - |                               |
	  - |_bottom of screen______________|
	  
	  methods:
	  - demarcate: sets tracking variables without modifying screen contents
	  - resize: updates tracking variables and modifies screen buffer appropriately
	*/
	
	class screen_space{
	protected:
		screen_space * const next;
		
		mutable int y;
		bool is_visible; // not used in this class, but commonly used among subclasses
		
		void shift_y(int delta_y) const;
		
	public:
		screen_space(screen_space *init_next);
		
		void demarcate(int init_height) const;
		void resize(int new_height) const;
		
		void clear() const;
	};
	
#endif