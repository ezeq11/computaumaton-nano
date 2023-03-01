#include "compile_config.hpp"
#include "curses.hpp"

#ifndef ARDUINO_NANO_BUILD
	#include <cstring>
#endif

#include "draw.hpp"
#include "tuple_set.hpp"

// ------------------------------------------------------------ ||
void print_tuple(const tuple_layout *layout,const symb *tuple){
	if(layout->N > 1){
		addch('(');
	}
	
	for(uint j = 0;j < layout->N;++j){
		addch(ascii(tuple[j]));
		
		if(j + 1 == layout->TRANSITION_POS){
			addch(')');
			addch('-');
			addch('>');
			addch('(');
			
		}else if(j + 1 < layout->N && j < layout->TRANSITION_POS + layout->NONVAR_COUNT){
			addch(',');
		}
	}
	
	if(layout->N > 1){
		addch(')');
	}
}

// ------------------------------------------------------------ ||
tuple_layout::tuple_layout(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,uint INIT_BLOCK_SIZE,bool INIT_DRAW_VERTICAL):
	TRANSITION_POS(INIT_TRANSITION_POS > 3 ? 3 : INIT_TRANSITION_POS),NONVAR_COUNT(INIT_NONVAR_COUNT),N(INIT_N),BLOCK_SIZE(INIT_BLOCK_SIZE),
	DRAW_VERTICAL(INIT_DRAW_VERTICAL),
	WRAP_SIZE(INIT_DRAW_VERTICAL ? 8 : 16),TUPLE_PRINT_WIDTH(
		(INIT_TRANSITION_POS == 0 ? 0 : 3) + (INIT_N == 0 ? 0 : (INIT_TRANSITION_POS + INIT_NONVAR_COUNT < INIT_N ? INIT_TRANSITION_POS + INIT_NONVAR_COUNT : INIT_N - 1)) + INIT_N + (INIT_N > 1 ? 2 : 0)
	) // see print_tuple
{
	
}

// ------------------------------------------------------------ ||
// Data methods ------------------------------------- |
#define EDIT_PREAMBLE  bool redraw = false
#define EDIT_ASSERT    redraw = true
#define EDIT_POSTAMBLE if(redraw){ draw(); }

const symb *tuple_set::comparison_buffer;
const tuple_set *tuple_set::containing_superset;
symb tuple_set::contained_val;

void tuple_set::remove_if(bool (tuple_set::*remove_tuple)(uint) const){
	EDIT_PREAMBLE;
	
	// O(n) algorithm: copy only if not deleted
	uint dest_i = 0;
	
	for(uint src_i = 0;src_i < len;++src_i){
		if((this->*remove_tuple)(src_i)){
			// Skip if to be removed
			EDIT_ASSERT;
			continue;
		}
		
		// Otherwise, copy
		if(dest_i != src_i){
			memcpy(block + dest_i,block + src_i,layout->N * sizeof(symb));
		}
		
		++dest_i;
	}
	
	// Done!
	len = dest_i;
	EDIT_POSTAMBLE;
}

bool tuple_set::tuple_equals_buffer(uint i) const{
	return memcmp(block + (i * layout->N),comparison_buffer,layout->N * sizeof(symb)) == 0;
}

bool tuple_set::tuple_contains(uint i) const{
	for(uint j = 0;j < layout->N;++j){
		if(supersets[j] == containing_superset && block[i * layout->N + j] == contained_val){
			return true;
		}
	}
	
	return false;
}

void tuple_set::on_add(const symb *tuple){
	// Verify available space
	if(len >= layout->BLOCK_SIZE){
		return;
	}
	
	// Verify uniqueness
	for(uint i = 0;i < len;++i){
		if(memcmp(block + (i * layout->N),tuple,layout->N * sizeof(symb)) == 0){
			return;
		}
	}
	
	// Locate insertion position according to dictionary order
	uint insert_i = 0;
	uint comparison_j = 0;
	
	while(comparison_j < layout->N){
		while(insert_i < len && block[insert_i * layout->N + comparison_j] < tuple[comparison_j]){
			++insert_i;
		}
		
		++comparison_j;
	}
	
	// Insert
	EDIT_PREAMBLE;
	
	memmove(block + ((insert_i + 1) * layout->N),block + (insert_i * layout->N),(len - insert_i) * layout->N * sizeof(symb));
	memcpy(block + (insert_i * layout->N),tuple,layout->N * sizeof(symb));
	
	++len;
	
	EDIT_ASSERT;
	EDIT_POSTAMBLE;
}

void tuple_set::on_remove(const symb *tuple){
	comparison_buffer = tuple;
	remove_if(&tuple_set::tuple_equals_buffer);
	
	// Handle monad-set dependencies
	if(layout->N == 1){
		monad_set_on_remove_callback(this,tuple[0]);
	}
}

void tuple_set::on_set(const symb *tuple){
	if(layout->BLOCK_SIZE < 1){
		return;
	}
	
	EDIT_PREAMBLE;
	
	memcpy(block,tuple,layout->N * sizeof(symb));
	len = 1;
	
	EDIT_ASSERT;
	EDIT_POSTAMBLE;
}

void tuple_set::on_clear(){
	EDIT_PREAMBLE;
	
	len = 0;
	
	EDIT_ASSERT;
	EDIT_POSTAMBLE;
}

void tuple_set::remove_containing(const tuple_set *superset,symb val){
	containing_superset = superset;
	contained_val = val;
	
	remove_if(&tuple_set::tuple_contains);
	
	// Handle monad-set dependencies
	if(layout->N == 1){
		monad_set_on_remove_callback(this,val);
	}
}

bool tuple_set::contains(symb val) const{
	if(layout->N != 1){
		// The monad (val) is a 1-tuple
		return false;
	}
	
	for(uint i = 0;i < len;++i){
		if(block[i] == val){
			return true;
		}
	}
	
	return false;
}

uint tuple_set::size() const{
	return len;
}

// Draw methods ------------------------------------- |
#define CONTENTS_HEIGHT (layout->DRAW_VERTICAL ? (len < layout->WRAP_SIZE ? len : layout->WRAP_SIZE) : (len / layout->WRAP_SIZE) + (len % layout->WRAP_SIZE > 0 ? 1 : 0))
#define CONTENTS_WIDTH  (layout->DRAW_VERTICAL ? (len / layout->WRAP_SIZE) + (len % layout->WRAP_SIZE > 0 ? 1 : 0) : (len < layout->WRAP_SIZE ? len : layout->WRAP_SIZE))

#define calculate_contents_height(var) CONTENTS_HEIGHT; var = (var == 0 ? 1 : var)
#define calculate_contents_width(var)  CONTENTS_WIDTH

#define calculate_component_height(var) calculate_contents_height(var); var += 1

void tuple_set::move_to_header_prefix() const{
	move(screen_space::top(),INDENT_X);
}

void tuple_set::move_to_read_position() const{
	int y = screen_space::top();
	int x = INDENT_X + 7 + (layout->BLOCK_SIZE > 1 ? 2 : 0);
	
	if(are_contents_shown){
		uint contents_height = calculate_contents_height(contents_height);
		uint contents_width = calculate_contents_width(contents_width);
		
		move(y + contents_height - 1,x + contents_width * (layout->TUPLE_PRINT_WIDTH + 2) + (contents_height == 1 ? 0 : 1));
	}else{
		move(y,x);
	}
}

void tuple_set::move_to_ith_suffix(uint i) const{
	int y = screen_space::top();
	int x = INDENT_X + 7 + (layout->BLOCK_SIZE > 1 ? 2 : 0);
	
	if(layout->DRAW_VERTICAL){
		y += i % layout->WRAP_SIZE;
		x += (i / layout->WRAP_SIZE) * (layout->TUPLE_PRINT_WIDTH + 2);
	}else{
		y += i / layout->WRAP_SIZE;
		x += (i % layout->WRAP_SIZE) * (layout->TUPLE_PRINT_WIDTH + 2);
	}
	
	move(y,x);
}

void tuple_set::draw_show_contents(bool new_contents_shown){
	if(are_contents_shown == new_contents_shown){
		return;
	}
	
	are_contents_shown = new_contents_shown;
	draw();
}

void tuple_set::draw_apply_filter(symb (*new_filter)[3]){
	if(filter_applied == new_filter){
		return;
	}
	
	filter_applied = new_filter;
	draw();
}

void tuple_set::draw_set_visibility(bool new_visibility){
	if(is_visible == new_visibility){
		return;
	}
	
	is_visible = new_visibility;
	draw();
}

void tuple_set::demarcate() const{
	uint current_height = calculate_component_height(current_height);
	screen_space::demarcate(current_height);
}

void tuple_set::draw() const{
	uint current_height = calculate_component_height(current_height);
	screen_space::resize(current_height);
	screen_space::clear();
	
	if(!is_visible){
		return;
	}
	
	uint contents_height = calculate_contents_height(contents_height);
	uint contents_width = calculate_contents_width(contents_width);
	
	int y = screen_space::top();
	int x = INDENT_X;
	
	// Header
	move(y,x);
	
	addch(' ');
	addch(' ');
	
	addch(prefix_1);
	addch(prefix_2);
	addch(' ');
	addch('=');
	addch(' ');
	
	if(are_contents_shown){
		x += 7;
		
		if(layout->BLOCK_SIZE > 1){
			// Draw container
			addch('{');
			addch(' ');
			
			x += 2;
			
			move(y + contents_height - 1,x + contents_width * (layout->TUPLE_PRINT_WIDTH + 2) - (contents_height == 1 ? 1 : 0));
			addch('}');
		}
		
		// Draw contents
		for(uint row = 0;row < contents_height;++row){
			uint row_width;
			
			if(layout->DRAW_VERTICAL){
				row_width = (len / layout->WRAP_SIZE) + (row < (len % layout->WRAP_SIZE) ? 1 : 0);
			}else{
				row_width = (row + 1 == contents_height ? len % layout->WRAP_SIZE : layout->WRAP_SIZE);
			}
			
			move(y + row,x);
			
			for(uint column = 0;column < row_width;++column){
				uint k = (layout->DRAW_VERTICAL ? column * layout->WRAP_SIZE + row : row * layout->WRAP_SIZE + column);
				
				if(filter_applied != NULL && memcmp(block + k * layout->N,filter_applied,layout->TRANSITION_POS * sizeof(symb)) != 0){
					for(uint c = 0;c < layout->TUPLE_PRINT_WIDTH;++c){
						addch(' ');
					}
					
					if(k + 1 < len){
						addch(' ');
					}else if(contents_height > 1){
						addch(' ');
					}
					
					addch(' ');
				}else{
					print_tuple(layout,block + k * layout->N);
					
					if(k + 1 < len){
						addch(',');
					}else if(contents_height > 1){
						addch(' ');
					}
					
					addch(' ');
				}
			}
		}
	}
}

// OOP object management ---------------------------- |
tuple_set::tuple_set(screen_space *init_next,tuple_layout *init_layout,char init_prefix_1,char init_prefix_2,const tuple_set * * init_supersets,symb *init_block):
	screen_space(init_next),
	
	layout(init_layout),
	
	prefix_1(init_prefix_1),prefix_2(init_prefix_2),
	supersets(init_supersets),
	
	len(0),
	block(init_block),
	
	is_visible(true),
	are_contents_shown(true),
	filter_applied(NULL)
{
	
}

void tuple_set::set_superset(uint j,const tuple_set *superset){
	supersets[j] = superset;
}

// ------------------------------------------------------------ ||
void tuple_set_operations::clear_edit_indicator() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_header_prefix();
	addch(' ');
}

void tuple_set_operations::draw_edit_indicator() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_header_prefix();
	addch('>');
}

void tuple_set_operations::clear_edit_read() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_read_position();
	clrtoeol();
}

void tuple_set_operations::draw_edit_read() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_read_position();
	
	switch(current_read){
	case READ_IDEMPOTENT:
		clrtoeol();
		
		break;
	case READ_ADD:
	case READ_REMOVE:
		addch(' ');
		addch(current_read == READ_ADD ? 'U' : '\\');
		addch(' ');
		
		// fall through
	case READ_SET:
		if(focus->layout->BLOCK_SIZE > 1){
			addch('{');
			addch(' ');
		}
		
		print_tuple(focus->layout,buffer);
		
		if(focus->layout->BLOCK_SIZE > 1){
			addch(' ');
			addch('}');
		}
		
		break;
	}
}

void tuple_set_operations::clear_filter_nav() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_ith_suffix(nav);
	addch(' ');
}

void tuple_set_operations::draw_filter_nav() const{
	if(focus == NULL || focus->filter_applied != &vals){
		return;
	}
	
	focus->move_to_ith_suffix(nav);
	addch(results_count <= 1 ? ' ' : '<');
}

void tuple_set_operations::edit_read_init(read new_read){
	if(current_operation != OPERATION_EDIT){
		return;
	}
	
	current_read = new_read;
	pos = 0;
	memset(buffer,SYMBOL_COUNT,MAX_N * sizeof(symb));
	
	draw_edit_read();
}

tuple_set_operations::tuple_set_operations()
:focus(NULL),current_operation(OPERATION_NIL){
	
}

bool tuple_set_operations::switch_available() const{
	switch(current_operation){
	case OPERATION_NIL:
		return true;
		
	case OPERATION_EDIT:
		return current_read == READ_IDEMPOTENT;
		
	case OPERATION_FILTER:
		return true;
		
	}
	
	return false;
}

void tuple_set_operations::switch_to(tuple_set *new_focus,operation new_operation){
	if(!switch_available()){
		return;
	}
	
	switch(current_operation){
	case OPERATION_NIL:
		
		break;
	case OPERATION_EDIT:
		clear_edit_indicator();
		clear_edit_read();
		
		break;
	case OPERATION_FILTER:
		filter_clear();
		
		break;
	}
	
	focus = new_focus;
	current_operation = new_operation;
	
	switch(current_operation){
	case OPERATION_NIL:
		
		break;
	case OPERATION_EDIT:
		edit_read_init(READ_IDEMPOTENT);
		
		draw_edit_indicator();
		draw_edit_read();
		
		break;
	case OPERATION_FILTER:
		
		break;
	}
}

void tuple_set_operations::draw() const{
	switch(current_operation){
	case OPERATION_NIL:
		
		break;
	case OPERATION_EDIT:
		draw_edit_indicator();
		draw_edit_read();
		
		break;
	case OPERATION_FILTER:
		draw_filter_nav();
		
		break;
	}
}

void tuple_set_operations::edit(int in){
	if(focus == NULL || current_operation != OPERATION_EDIT){
		return;
	}
	
	if(current_read == READ_IDEMPOTENT){
		switch(in){
		case 'u':
		case 'U':
			if(focus->layout->BLOCK_SIZE > 1){
				edit_read_init(READ_ADD);
			}
			
			break;
		case '\\':
			if(focus->layout->BLOCK_SIZE > 1){
				edit_read_init(READ_REMOVE);
			}
			
			break;
		case '=':
			if(focus->layout->BLOCK_SIZE == 1){
				edit_read_init(READ_SET);
				focus->draw_show_contents(false);
			}
			
			break;
		case '/':
			if(focus->layout->BLOCK_SIZE == 1){
				focus->on_clear();
			}
			
			break;
		}
	}else{
		switch(in){
		case '\b':
		case 0x7f:
			if(pos == 0){
				break;
			}
			
			--pos;
			buffer[pos] = SYMBOL_COUNT;
			
			draw_edit_read();
			
			break;
		case '\t':
		case '\n':
		case '\r':
			if(pos < focus->layout->TRANSITION_POS + focus->layout->NONVAR_COUNT){
				break;
			}
			
			switch(current_read){
			case READ_IDEMPOTENT:
				break;
			case READ_ADD:
				focus->on_add(buffer);
				
				break;
			case READ_REMOVE:
				focus->on_remove(buffer);
				
				break;
			case READ_SET:
				focus->on_set(buffer);
				focus->draw_show_contents(true);
				
				break;
			}
			
			if(in == '\t' && (current_read == READ_ADD || current_read == READ_REMOVE)){
				edit_read_init(current_read);
			}else{
				edit_read_init(READ_IDEMPOTENT);
			}
			
			break;
		case '`':
			if(current_read == READ_SET){
				focus->draw_show_contents(true);
			}
			
			edit_read_init(READ_IDEMPOTENT);
			
			break;
		default:
			if(in > (char)in){
				break;
			}
			
			if(pos < focus->layout->N && is_symbol((char)in)){
				symb new_val = symbol((char)in);
				
				if(focus->supersets[pos] == NULL || focus->supersets[pos]->contains(new_val)){
					buffer[pos] = new_val;
					++pos;
					
					draw_edit_read();
				}
			}
			
			break;
		}
	}
}

const tuple_set *tuple_set_operations::edit_current_superset() const{
	if(focus == NULL || current_operation != OPERATION_EDIT){
		return NULL;
	}
	
	return focus->supersets[pos];
}

void tuple_set_operations::filter_clear(){
	if(focus == NULL || current_operation != OPERATION_FILTER){
		return;
	}
	
	focus->draw_apply_filter(NULL);
	clear_filter_nav();
}

void tuple_set_operations::filter_apply(symb val_0,symb val_1,symb val_2){
	if(focus == NULL || current_operation != OPERATION_FILTER){
		return;
	}
	
	// Record values
	vals[0] = val_0;
	vals[1] = val_1;
	vals[2] = val_2;
	
	// Count results
	results_count = 0;
	
	for(uint i = 0;i < focus->len;++i){
		if(memcmp(focus->block + i * focus->layout->N,vals,focus->layout->TRANSITION_POS * sizeof(symb)) == 0){
			++results_count;
		}
	}
	
	// Set navigation index
	nav = 0;
	
	if(results_count > 0 && memcmp(focus->block + nav * focus->layout->N,vals,focus->layout->TRANSITION_POS * sizeof(symb)) != 0){
		filter_nav_next();
	}
	
	// Done!
	focus->draw_apply_filter(&vals);
	draw_filter_nav();
}

uint tuple_set_operations::filter_results() const{
	if(focus == NULL || current_operation != OPERATION_FILTER){
		return 0;
	}
	
	return results_count;
}

void tuple_set_operations::filter_nav_next(){
	if(focus == NULL || current_operation != OPERATION_FILTER || results_count == 0){
		return;
	}
	
	clear_filter_nav();
	
	uint new_nav = nav + 1;
	
	while(new_nav < focus->len && memcmp(focus->block + new_nav * focus->layout->N,vals,focus->layout->TRANSITION_POS * sizeof(symb)) != 0){
		++new_nav;
	}
	
	if(new_nav < focus->len){
		nav = new_nav;
	}
	
	draw_filter_nav();
}

void tuple_set_operations::filter_nav_prev(){
	if(focus == NULL || current_operation != OPERATION_FILTER || results_count == 0){
		return;
	}
	
	clear_filter_nav();
	
	uint new_nav = (nav == 0 ? 0 : nav - 1);
	
	while(new_nav > 0 && memcmp(focus->block + new_nav * focus->layout->N,vals,focus->layout->TRANSITION_POS * sizeof(symb)) != 0){
		--new_nav;
	}
	
	if(memcmp(focus->block + new_nav * focus->layout->N,vals,focus->layout->TRANSITION_POS * sizeof(symb)) == 0){
		nav = new_nav;
	}
	
	draw_filter_nav();
}

const symb *tuple_set_operations::filter_nav_select() const{
	if(focus == NULL || current_operation != OPERATION_FILTER || results_count == 0){
		return NULL;
	}
	
	return focus->block + nav * focus->layout->N;
}

// ------------------------------------------------------------ ||
tuple_layout set::layout(0,1,1,SET_BLOCK_SIZE,false);

set::set(screen_space *init_next,char init_prefix_1,char init_prefix_2)
:tuple_set(init_next,&layout,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),supersets{}{
	
}

// ------------------------------------------------------------ ||
tuple_layout element::layout(0,1,1,1,true);

element::element(screen_space *init_next,char init_prefix_1,char init_prefix_2)
:tuple_set(init_next,&layout,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),supersets{}{
	
}

bool element::is_set() const{
	return size() > 0;
}

symb element::get() const{
	return size() > 0 ? block[0] : SYMBOL_COUNT;
}

// ------------------------------------------------------------ ||
product::product(screen_space *init_next,uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,char init_prefix_1,char init_prefix_2):
	tuple_set(init_next,&layout,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),
	
	layout(INIT_TRANSITION_POS,INIT_NONVAR_COUNT,INIT_N,PRODUCT_BLOCK_SIZE / INIT_N,true),
	supersets{}
{
	
}
