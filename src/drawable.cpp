class drawable{
private:
	drawable * const next;
	
	virtual void init_draw_internal(int y,int x) const = 0;
	virtual void re_draw_internal(int y) const = 0;
	virtual uint height() const = 0;
	
protected:
	mutable int draw_x;
	mutable uint prev_height;
	
public:
	drawable(drawable * init_next);
	
	int init_draw(int y,int x) const;
	int re_draw(int y) const;
};

drawable::drawable(drawable * init_next)
:next(init_next){
	
}

int drawable::init_draw(int y,int x) const{
	init_draw_internal(y,x);
	draw_x = x;
	
	prev_height = height();
	y += prev_height;
	
	if(next == NULL){
		return y;
	}else{
		return next->init_draw(y,x);
	}
}

int drawable::re_draw(int y) const{
	uint current_height = height();
	
	if(prev_height != current_height){
		// Adjust draw space
		
		prev_height = current_height;
	}
	
	// Draw on top of this
	re_draw_internal(y);
	y += current_height;
	
	if(next == NULL){
		return y;
	}else{
		return next->re_draw(y);
	}
}