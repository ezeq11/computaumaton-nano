#include "curses.hpp"
#include "element.hpp"

element::element(char name_1,char name_2)
:ELEMENT_COMPONENT(name_1,name_2),val(SYMBOL_COUNT){
	
}

// Edit ------------------------------------------------------- ||
void element::on_add(){
	// Not implemented
}

void element::on_remove(){
	// Not implemented
}

void element::on_set(){
	val = buffer[0];
}

void element::on_clear(){
	val = SYMBOL_COUNT;
}

void element::remove_containing(const set *superset,symb to_remove){
	if(supersets[0] == superset && val == to_remove){
		val = SYMBOL_COUNT;
	}
}

// Draw ------------------------------------------------------- ||
int element::draw_contents(int y,int x,int *cx,bool indicate_current_item) const{
	addch(ascii(val));
	*cx += 1;
	
	return y;
}

void element::draw_buffer(int *cx) const{
	addch(ascii(buffer[0]));
	*cx += pos;
}

int element::nodraw_contents(int y) const{
	return y;
}

// Specialized ------------------------------------------------ ||
bool element::is_set() const{
	return val != SYMBOL_COUNT;
}

symb element::get() const{
	return val;
}