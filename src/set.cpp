#include "set.hpp"

set::set(char name_1,char name_2,void (*remove_callback)(const set *,symb))
:SET_COMPONENT(name_1,name_2),members(),members_iter(&members),on_remove_callback(remove_callback){
	
}

// Edit ------------------------------------------------------- ||
uint set::size() const{
	return members.count(0,SYMBOL_COUNT);
}

void set::on_add(){
	members.add(buffer[0]);
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

// Draw ------------------------------------------------------- ||
bool set::horizontal_iter_begin() const{
	return members_iter.begin();
}

bool set::horizontal_iter_seek() const{
	return members_iter.seek_next();
}

bool set::horizontal_iter_is_current() const{
	return false;
}

bool set::horizontal_iter_is_visible() const{
	return true;
}

symb set::horizontal_iter_get(uint j) const{
	return (symb)members_iter.get();
}

bool set::vertical_is_current(uint i) const{
	// Not implemented
	return false;
}

bool set::vertical_is_visible(uint i) const{
	// Not implemented
	return false;
}

symb set::vertical_get(uint i,uint j) const{
	// Not implemented
	return SYMBOL_COUNT;
}

// Specialized ------------------------------------------------ ||
bool set::contains(symb val) const{
	return members.get(val);
}