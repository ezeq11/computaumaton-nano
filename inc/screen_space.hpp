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
	  - S1.top(): returns S1.y
	  
	  - S1.collapse(new_top): sets S1.y = new_top, S2.y = new_top, S3.y = new_top
	  - S1.demarcate(init_height): moves S2.y to S1.y + new_height, shifts S3.y by same distance
	  - S1.resize(new_height): same as demarcate(), except screen contents are preserved and moved
	  
	  - S1.clear(): clears area reserved for S1 contents
	*/
	
	class screen_space{
	private:
		screen_space * const next;
		mutable int y;
		
		void shift_y(int delta_y) const;
		
	public:
		screen_space(screen_space *init_next);
		
		int top() const;
		
		void collapse(int new_top) const;
		void demarcate(int init_height) const;
		void resize(int new_height) const;
		
		void clear() const;
	};
	
#endif