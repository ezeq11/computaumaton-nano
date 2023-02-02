#include "element.hpp"

element::element(char name_1,char name_2)
:ELEMENT_COMPONENT(name_1,name_2),val(SYMBOL_COUNT){
	
}

// Edit ------------------------------------------------------- ||
uint element::size() const{
	return (uint)(val != SYMBOL_COUNT);
}

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

// Draw ------------------------------------------------------- ||
bool element::horizontal_iter_begin() const{
	// Not implemented
	return false;
}

bool element::horizontal_iter_seek() const{
	// Not implemented
	return false;
}

bool element::horizontal_iter_is_current() const{
	// Not implemented
	return false;
}

bool element::horizontal_iter_is_visible() const{
	// Not implemented
	return false;
}

symb element::horizontal_iter_get(uint j) const{
	// Not implemented
	return SYMBOL_COUNT;
}

bool element::vertical_is_current(uint i) const{
	return false;
}

bool element::vertical_is_visible(uint i) const{
	return true;
}

symb element::vertical_get(uint i,uint j) const{
	return val;
}

// Specialized ------------------------------------------------ ||
bool element::is_set() const{
	return val != SYMBOL_COUNT;
}

symb element::get() const{
	return val;
}