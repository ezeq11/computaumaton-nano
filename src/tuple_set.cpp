#include "compile_config.hpp"
#include "curses.hpp"

#ifndef ARDUINO_NANO_BUILD
	#include <cstring>
#endif

#include "draw.hpp"
#include "tuple_set.hpp"

// ------------------------------------------------------------ ||
void print_tuple(const tuple_config *config,const symb *tuple){
	if(config->N > 1){
		addch('(');
	}
	
	for(uint j = 0;j < config->N;++j){
		addch(ascii(tuple[j]));
		
		if(j + 1 == config->TRANSITION_POS){
			addch(')');
			addch('-');
			addch('>');
			addch('(');
			
		}else if(j + 1 < config->N && j < config->TRANSITION_POS + config->NONVAR_COUNT){
			addch(',');
		}
	}
	
	if(config->N > 1){
		addch(')');
	}
}

// ------------------------------------------------------------ ||
tuple_config::tuple_config(uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,uint INIT_BLOCK_SIZE,bool INIT_DRAW_VERTICAL):
	TRANSITION_POS(INIT_TRANSITION_POS > 3 ? 3 : INIT_TRANSITION_POS),NONVAR_COUNT(INIT_NONVAR_COUNT),N(INIT_N),BLOCK_SIZE(INIT_BLOCK_SIZE),
	DRAW_VERTICAL(INIT_DRAW_VERTICAL),
	WRAP_SIZE(INIT_DRAW_VERTICAL ? 8 : 16),TUPLE_PRINT_WIDTH(
		(INIT_TRANSITION_POS == 0 ? 0 : 3) + (INIT_N == 0 ? 0 : (INIT_TRANSITION_POS + INIT_NONVAR_COUNT < INIT_N ? INIT_TRANSITION_POS + INIT_NONVAR_COUNT : INIT_N - 1)) + INIT_N + (INIT_N > 1 ? 2 : 0)
	) // see print_tuple
{
	
}

// ------------------------------------------------------------ ||
filter_store::filter_store()
:applied(false){
	
}

// ------------------------------------------------------------ ||
// Component dimensions ----------------------------- |
#define CONTENTS_HEIGHT (config->DRAW_VERTICAL ? (len < config->WRAP_SIZE ? len : config->WRAP_SIZE) : (len / config->WRAP_SIZE) + (len % config->WRAP_SIZE > 0 ? 1 : 0))
#define CONTENTS_WIDTH  (config->DRAW_VERTICAL ? (len / config->WRAP_SIZE) + (len % config->WRAP_SIZE > 0 ? 1 : 0) : (len < config->WRAP_SIZE ? len : config->WRAP_SIZE))

#define calculate_contents_height(var) CONTENTS_HEIGHT; var = (var == 0 ? 1 : var)
#define calculate_contents_width(var)  CONTENTS_WIDTH

#define calculate_component_height(var) calculate_contents_height(var); var += 1

// Edit methods ------------------------------------- |
// Variables -------
const symb *tuple_set::comparison_buffer;

const tuple_set *tuple_set::containing_superset;
symb tuple_set::contained_val;

// Methods ---------
#define EDIT_PREAMBLE  uint prev_height = calculate_component_height(prev_height); bool redraw = false
#define EDIT_ASSERT    redraw = true
#define EDIT_POSTAMBLE if(redraw){ re_draw(prev_height); }

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
			memcpy(block + dest_i,block + src_i,config->N * sizeof(symb));
		}
		
		++dest_i;
	}
	
	// Done!
	len = dest_i;
	EDIT_POSTAMBLE;
}

bool tuple_set::tuple_equals_buffer(uint i) const{
	return memcmp(block + (i * config->N),comparison_buffer,config->N * sizeof(symb)) == 0;
}

bool tuple_set::tuple_contains(uint i) const{
	for(uint j = 0;j < config->N;++j){
		if(supersets[j] == containing_superset && block[i * config->N + j] == contained_val){
			return true;
		}
	}
	
	return false;
}

void tuple_set::on_add(const symb *tuple){
	// Verify available space
	if(len >= config->BLOCK_SIZE){
		return;
	}
	
	// Verify uniqueness
	for(uint i = 0;i < len;++i){
		if(memcmp(block + (i * config->N),tuple,config->N * sizeof(symb)) == 0){
			return;
		}
	}
	
	// Locate insertion position according to dictionary order
	uint insert_i = 0;
	uint comparison_j = 0;
	
	while(comparison_j < config->N){
		while(insert_i < len && block[insert_i * config->N + comparison_j] < tuple[comparison_j]){
			++insert_i;
		}
		
		++comparison_j;
	}
	
	// Insert
	EDIT_PREAMBLE;
	
	memmove(block + ((insert_i + 1) * config->N),block + (insert_i * config->N),(len - insert_i) * config->N * sizeof(symb));
	memcpy(block + (insert_i * config->N),tuple,config->N * sizeof(symb));
	
	++len;
	
	EDIT_ASSERT;
	EDIT_POSTAMBLE;
}

void tuple_set::on_remove(const symb *tuple){
	comparison_buffer = tuple;
	remove_if(&tuple_set::tuple_equals_buffer);
	
	// Handle monad-set dependencies
	if(config->N == 1){
		monad_set_on_remove_callback(this,tuple[0]);
	}
}

void tuple_set::on_set(const symb *tuple){
	if(config->BLOCK_SIZE < 1){
		return;
	}
	
	EDIT_PREAMBLE;
	
	memcpy(block,tuple,config->N * sizeof(symb));
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
	if(config->N == 1){
		monad_set_on_remove_callback(this,val);
	}
}

bool tuple_set::contains(symb val) const{
	if(config->N != 1){
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

// Filter methods ----------------------------------- |
void tuple_set::filter_clear(){
	if(filter == NULL || filter->applied == false){
		return;
	}
	
	filter->applied = false;
	re_draw();
}

void tuple_set::filter_apply(symb filter_1,symb filter_2,symb filter_3){
	if(filter == NULL){
		return;
	}
	
	// Record values
	filter->vals[0] = filter_1;
	filter->vals[1] = filter_2;
	filter->vals[2] = filter_3;
	
	// Count results
	filter->results_count = 0;
	
	for(uint i = 0;i < len;++i){
		if(memcmp(block + i * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb)) == 0){
			++filter->results_count;
		}
	}
	
	// Set navigation index
	filter->nav = 0;
	
	if(filter->results_count > 0 && memcmp(block + filter->nav * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb) != 0)){
		filter_nav_next();
	}
	
	// Done!
	filter->applied = true;
	re_draw();
}

uint tuple_set::filter_results() const{
	if(filter == NULL || !filter->applied){
		return 0;
	}
	
	return filter->results_count;
}

void tuple_set::filter_nav_next(){
	if(filter == NULL || !filter->applied || filter->results_count == 0){
		return;
	}
	
	uint new_filter_nav = filter->nav + 1;
	
	while(new_filter_nav < len && memcmp(block + new_filter_nav * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb)) != 0){
		++new_filter_nav;
	}
	
	if(new_filter_nav < len){
		filter->nav = new_filter_nav;
	}
}

void tuple_set::filter_nav_prev(){
	if(filter == NULL || !filter->applied || filter->results_count == 0){
		return;
	}
	
	uint new_filter_nav = (filter->nav == 0 ? 0 : filter->nav - 1);
	
	while(new_filter_nav > 0 && memcmp(block + new_filter_nav * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb)) != 0){
		--new_filter_nav;
	}
	
	if(memcmp(block + new_filter_nav * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb)) == 0){
		filter->nav = new_filter_nav;
	}
}

const symb *tuple_set::filter_nav_select() const{
	if(filter == NULL || !filter->applied || filter->results_count == 0){
		return NULL;
	}
	
	return block + filter->nav * config->N;
}

// Draw methods ------------------------------------- |
void tuple_set::shift_y(int delta_y) const{
	y += delta_y;
	
	if(next != NULL){
		next->shift_y(delta_y);
	}
}

void tuple_set::adjust_screen_space(uint prev_height) const{
	uint current_height = calculate_component_height(current_height);
	
	if(prev_height == current_height){
		return;
	}
	
	move(y,INDENT_X);
	
	if(current_height < prev_height){
		for(uint r = 0;r < (prev_height - current_height);++r){
			deleteln();
		}
		
	}else{
		for(uint r = 0;r < (current_height - prev_height);++r){
			insertln();
		}
	}
	
	if(next != NULL){
		next->shift_y((int)current_height - (int)prev_height);
	}
}

void tuple_set::clear_screen_space() const{
	uint current_height = calculate_component_height(current_height);
	
	for(uint r = 0;r < current_height;++r){
		move(y + r,INDENT_X);
		clrtoeol();
	}
}

void tuple_set::draw_screen_space() const{
	if(!is_visible){
		return;
	}
	
	uint contents_height = calculate_contents_height(contents_height);
	uint contents_width = calculate_contents_width(contents_width);
	
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
		
		if(config->BLOCK_SIZE > 1){
			// Draw container
			addch('{');
			addch(' ');
			
			x += 2;
			
			move(y + contents_height - 1,x + contents_width * (config->TUPLE_PRINT_WIDTH + 2) - (contents_height == 1 ? 1 : 0));
			addch('}');
		}
		
		// Draw contents
		for(uint row = 0;row < contents_height;++row){
			uint row_width;
			
			if(config->DRAW_VERTICAL){
				row_width = (len / config->WRAP_SIZE) + (row < (len % config->WRAP_SIZE) ? 1 : 0);
			}else{
				row_width = (row + 1 == contents_height ? len % config->WRAP_SIZE : config->WRAP_SIZE);
			}
			
			move(y + row,x);
			
			for(uint column = 0;column < row_width;++column){
				uint k = (config->DRAW_VERTICAL ? column * config->WRAP_SIZE + row : row * config->WRAP_SIZE + column);
				
				if(filter != NULL && filter->applied && memcmp(block + k * config->N,filter->vals,config->TRANSITION_POS * sizeof(symb)) != 0){
					for(uint c = 0;c < config->TUPLE_PRINT_WIDTH;++c){
						addch(' ');
					}
					
					if(k + 1 < len){
						addch(' ');
					}else if(contents_height > 1){
						addch(' ');
					}
					
					addch(' ');
				}else{
					print_tuple(config,block + k * config->N);
					
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

void tuple_set::move_to_read_position() const{
	int x = INDENT_X + 7 + (config->BLOCK_SIZE > 1 ? 2 : 0);
	
	if(are_contents_shown){
		uint contents_height = calculate_contents_height(contents_height);
		uint contents_width = calculate_contents_width(contents_width);
		
		move(y + contents_height - 1,x + contents_width * (config->TUPLE_PRINT_WIDTH + 2) + (contents_height == 1 ? 0 : 1));
	}else{
		move(y,x);
	}
}

void tuple_set::re_draw() const{
	clear_screen_space();
	draw_screen_space();
}

void tuple_set::re_draw(uint prev_height) const{
	adjust_screen_space(prev_height);
	clear_screen_space();
	draw_screen_space();
}

void tuple_set::init_draw(int draw_y) const{
	y = draw_y;
	draw_screen_space();
	
	if(next != NULL){
		uint current_height = calculate_component_height(current_height);
		next->init_draw(y + current_height);
	}
}

void tuple_set::set_visibility(bool new_visibility){
	if(is_visible == new_visibility){
		return;
	}
	
	is_visible = new_visibility;
	re_draw();
}

void tuple_set::show_contents(bool new_contents_shown){
	if(are_contents_shown == new_contents_shown){
		return;
	}
	
	are_contents_shown = new_contents_shown;
	re_draw();
}

// OOP object management ---------------------------- |
tuple_set::tuple_set(tuple_config *init_config,filter_store *init_filter,tuple_set *init_next,char init_prefix_1,char init_prefix_2,const tuple_set * * init_supersets,symb *init_block):
	config(init_config),
	filter(init_filter),
	
	next(init_next),
	prefix_1(init_prefix_1),prefix_2(init_prefix_2),
	supersets(init_supersets),
	
	len(0),
	block(init_block),
	
	is_visible(true),
	are_contents_shown(true),
	y(0)
{
	
}

void tuple_set::set_superset(uint j,const tuple_set *superset){
	supersets[j] = superset;
}

// ------------------------------------------------------------ ||
void tuple_set_editor::draw() const{
	if(focus == NULL){
		return;
	}
	
	focus->move_to_read_position();
	
	switch(state){
	case READ_IDEMPOTENT:
		clrtoeol();
		
		break;
	case READ_ADD:
	case READ_REMOVE:
		addch(' ');
		addch(state == READ_ADD ? 'U' : '\\');
		addch(' ');
		
		// fall through
	case READ_SET:
		if(focus->config->BLOCK_SIZE > 1){
			addch('{');
			addch(' ');
		}
		
		print_tuple(focus->config,buffer);
		
		if(focus->config->BLOCK_SIZE > 1){
			addch(' ');
			addch('}');
		}
		
		break;
	}
}

void tuple_set_editor::init_read(read new_state){
	state = new_state;
	pos = 0;
	memset(buffer,SYMBOL_COUNT,MAX_N * sizeof(symb));
	
	draw();
}

tuple_set_editor::tuple_set_editor()
:focus(NULL){
	init_read(READ_IDEMPOTENT);
}

void tuple_set_editor::switch_to(tuple_set *new_focus){
	if(pos != 0){
		return;
	}
	
	if(focus != NULL){
		focus->move_to_read_position();
		clrtoeol();
	}
	
	focus = new_focus;
	draw();
}

void tuple_set_editor::edit(int in){
	if(focus == NULL){
		return;
	}
	
	if(state == READ_IDEMPOTENT){
		switch(in){
		case 'u':
		case 'U':
			if(focus->config->BLOCK_SIZE > 1){
				init_read(READ_ADD);
			}
			
			break;
		case '\\':
			if(focus->config->BLOCK_SIZE > 1){
				init_read(READ_REMOVE);
			}
			
			break;
		case '=':
			if(focus->config->BLOCK_SIZE == 1){
				init_read(READ_SET);
				focus->show_contents(false);
			}
			
			break;
		case '/':
			if(focus->config->BLOCK_SIZE == 1){
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
			
			draw();
			
			break;
		case '\t':
		case '\n':
		case '\r':
			if(pos < focus->config->TRANSITION_POS + focus->config->NONVAR_COUNT){
				break;
			}
			
			switch(state){
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
				focus->show_contents(true);
				
				break;
			}
			
			if(in == '\t' && (state == READ_ADD || state == READ_REMOVE)){
				init_read(state);
			}else{
				init_read(READ_IDEMPOTENT);
			}
			
			break;
		case '`':
			if(state == READ_SET){
				focus->show_contents(true);
			}
			
			init_read(READ_IDEMPOTENT);
			
			break;
		default:
			if(in > (char)in){
				break;
			}
			
			if(pos < focus->config->N && is_symbol((char)in)){
				symb new_val = symbol((char)in);
				
				if(focus->supersets[pos] == NULL || focus->supersets[pos]->contains(new_val)){
					buffer[pos] = new_val;
					++pos;
					
					draw();
				}
			}
			
			break;
		}
	}
}

bool tuple_set_editor::is_amid_edit() const{
	return state != READ_IDEMPOTENT;
}

const tuple_set *tuple_set_editor::get_superset_current() const{
	if(focus == NULL){
		return NULL;
	}
	
	return focus->supersets[pos];
}

// ------------------------------------------------------------ ||
tuple_config set::config(0,1,1,SET_BLOCK_SIZE,false);

set::set(tuple_set *init_next,char init_prefix_1,char init_prefix_2)
:tuple_set(&config,NULL,init_next,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),supersets{}{
	
}

// ------------------------------------------------------------ ||
tuple_config element::config(0,1,1,1,true);

element::element(tuple_set *init_next,char init_prefix_1,char init_prefix_2)
:tuple_set(&config,NULL,init_next,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),supersets{}{
	
}

bool element::is_set() const{
	return len > 0;
}

symb element::get() const{
	return len > 0 ? block[0] : SYMBOL_COUNT;
}

// ------------------------------------------------------------ ||
product::product(tuple_set *init_next,uint INIT_TRANSITION_POS,uint INIT_NONVAR_COUNT,uint INIT_N,char init_prefix_1,char init_prefix_2):
	tuple_set(&config,&filter,init_next,init_prefix_1,init_prefix_2,(const tuple_set **)&supersets,(symb *)&block),
	config(INIT_TRANSITION_POS,INIT_NONVAR_COUNT,INIT_N,PRODUCT_BLOCK_SIZE / INIT_N,true),
	filter(),
	supersets{}
{
	
}
