#include "set.hpp"

set::set(char name_1,char name_2,void (*remove_callback)(const set *,symb))
:SET_COMPONENT(name_1,name_2),members(),members_iter(&members),on_remove_callback(remove_callback){
	
}

// Edit ------------------------------------------------------- ||
uint set::size() const{
	return members.size();
}

void set::on_add(){
	members.add(buffer[0]);
	members.mask_range(0,SYMBOL_COUNT);
}

void set::on_remove(){
	members.remove(buffer[0]);
	
	if(on_remove_callback != NULL){
		on_remove_callback(this,buffer[0]);
	}
}

void set::on_set(){
	// Not implemented
}

void set::on_clear(){
	// Not implemented
}

void set::remove_containing(uint j,symb to_remove){
	members.remove(to_remove);
	
	if(on_remove_callback != NULL){
		on_remove_callback(this,to_remove);
	}
}

// Draw ------------------------------------------------------- ||
#define SET_WRAP_SIZE 8

int set::draw_contents(int y,int x,int *cx,bool indicate_current_item) const{
	bool indented = members.size() > SET_WRAP_SIZE;
	
	addch('{');
	addch(' ');
	*cx += 2;
	
	if(indented){
		++y;
		move(y,x + 2);
		
		*cx = x + 2;
	}
	
	bool draw_more = members_iter.begin();
	uint column = 0;
	
	while(draw_more){
		addch(ascii((symb)members_iter.get()));
		
		draw_more = members_iter.seek_next();
		++column;
		
		if(draw_more){
			addch(',');
		}else{
			addch(' ');
		}
		
		*cx += 2;
		
		if(draw_more){
			if(column < SET_WRAP_SIZE){
				addch(' ');
				*cx += 1;
			}else{
				++y;
				move(y,x + 2);
				
				column = 0;
				*cx = x + 2;
			}
		}
	}
	
	if(indented){
		y += 2;
		move(y,x);
		
		addch('}');
		*cx = x + 1;
	}else{
		addch('}');
		*cx += 1;
	}
	
	return y;
}

void set::draw_buffer(int *cx) const{
	addch('{');
	addch(' ');
	addch(ascii(buffer[0]));
	addch(' ');
	addch('}');
	
	*cx += 2 + pos;
}

int set::nodraw_contents(int y) const{
	uint len = members.size();
	
	if(len > SET_WRAP_SIZE){
		return y + 1 + (len / SET_WRAP_SIZE) + (len % SET_WRAP_SIZE > 0 ? 1 : 0) + 1;
	}else{
		return y;
	}
}

// Specialized ------------------------------------------------ ||
bool set::contains(symb val) const{
	return members.get(val);
}