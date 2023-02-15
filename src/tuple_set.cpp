#include "tuple_set.hpp"

// Edit methods ----------------------------------------------- ||
void tuple_set::init_read(read new_state){
	state = new_state;
	pos = 0;
	memset(buffer,SYMBOL_COUNT,N * sizeof(symb));
	
	redraw_read = true;
}

const tuple_set *tuple_set::containing_superset;
symb tuple_set::contained_val;

void tuple_set::remove_if(bool (*tuple_set::remove_tuple)(uint) const){
	// O(n) algorithm: copy only if not deleted
	uint dest_i = 0;
	
	for(uint src_i = 0;src_i < len;++src_i){
		if((this->*remove_tuple)(src_i)){
			// Skip if to be removed
			redraw_component = true;
			continue;
		}
		
		// Otherwise, copy
		if(dest_i != src_i){
			memcpy(block + dest_i,block + src_i,N * sizeof(symb));
		}
		
		++dest_i;
	}
	
	// Done!
	len = dest_i;
}

bool tuple_set::tuple_equals_buffer(uint i) const{
	return memcmp(block + (i * N),buffer,N * sizeof(symb)) == 0;
}

bool tuple_set::tuple_contains(uint i) const{
	for(uint j = 0;j < N;++j){
		if(supersets[j] == containing_superset && block[i * N + j] == contained_val){
			return true;
		}
	}
	
	return false;
}

void tuple_set::on_add(){
	// Verify available space
	if(len >= BLOCK_SIZE){
		return;
	}
	
	// Verify uniqueness
	for(uint i = 0;i < len;++i){
		if(memcmp(block + (i * N),buffer,N * sizeof(symb)) == 0){
			return;
		}
	}
	
	// Locate insertion position according to dictionary order
	uint insert_i = 0;
	uint comparison_j = 0;
	
	while(comparison_j < N){
		while(insert_i < len && block[insert_i * N + comparison_j] < buffer[comparison_j]){
			++insert_i;
		}
		
		++comparison_j;
	}
	
	// Insert
	memmove(block + ((insert_i + 1) * N),block + (insert_i * N),(len - insert_i) * N * sizeof(symb));
	memcpy(block + (insert_i * N),buffer,N * sizeof(symb));
	
	++len;
	
	redraw_component = true;
}

void tuple_set::on_remove(){
	remove_if(&tuple_set::tuple_equals_buffer);
}

void tuple_set::on_set(){
	if(BLOCK_SIZE < 1){
		return;
	}
	
	memcpy(block,buffer,N * sizeof(symb));
	len = 1;
	
	redraw_component = true;
}

void tuple_set::on_clear(){
	len = 0;
	
	redraw_component = true;
}

// Draw methods ----------------------------------------------- ||
uint tuple_set::contents_height(draw draw_mode) const{
	switch(draw_mode){
	case DRAW_INVALID:
		return 0;
		
	case DRAW_TUPLE:
	case DRAW_HORIZONTAL_SINGLE:
		return 1;
		
	case DRAW_HORIZONTAL_MULTI:
		return (len / WRAP_SIZE) + (len % WRAP_SIZE > 0 ? 1 : 0);
		
	case DRAW_VERTICAL:
		return (len < WRAP_SIZE ? len : WRAP_SIZE);
	}
	
	return 0;
}

uint tuple_set::height(draw draw_mode) const{
	switch(draw_mode){
	case DRAW_HORIZONTAL_MULTI:
	case DRAW_VERTICAL:
		
		return 1 + contents_height(draw_mode) + 1 + 2;
	case DRAW_INVALID:
	case DRAW_TUPLE:
	case DRAW_HORIZONTAL_SINGLE:
		
		break;
	}
	
	return 2;
}

void tuple_set::print_tuple(const symb *tuples,uint i) const{
	if(N > 1){
		addch('(');
	}
	
	for(uint j = 0;j < N;++j){
		addch(ascii(tuples[i * N + j]));
		
		if(j < NONVAR_N && j + 1 < N){
			addch(',');
		}
	}
	
	if(N > 1){
		addch(')');
	}
}

void tuple_set::draw_component(int y,int x,draw draw_mode) const{
	if(draw_mode == DRAW_INVALID || state == READ_SET){
		return;
	}
	
	// Draw container
	switch(draw_mode){
	case DRAW_INVALID:
	case DRAW_TUPLE:
		
		break;
	case DRAW_HORIZONTAL_SINGLE:
		move(y,x);
		addch('{');
		addch(' ');
		
		move(y,x + 2 + len * (TUPLE_PRINT_WIDTH + 2));
		addch('}');
		
		break;
	case DRAW_HORIZONTAL_MULTI:
	case DRAW_VERTICAL:
		move(y,x);
		addch('{');
		
		move(y + 1 + contents_height(draw_mode) + 1,x);
		addch('}');
		
		break;
	}
	
	// Draw contents
	uint height = contents_height(draw_mode);
	
	switch(draw_mode){
	case DRAW_INVALID:
		
		break;
	case DRAW_TUPLE:
		move(y,x);
		print_tuple(block,0);
		
		break;
	case DRAW_HORIZONTAL_SINGLE:
	case DRAW_HORIZONTAL_MULTI:
	case DRAW_VERTICAL:
		for(uint row = 0;row < height;++row){
			uint width;
			
			if(draw_mode == DRAW_VERTICAL){
				width = (len / WRAP_SIZE) + (row < (len % WRAP_SIZE) ? 1 : 0);
			}else{
				width = (row + 1 == height ? len % WRAP_SIZE : WRAP_SIZE);
			}
			
			move(y + (draw_mode == DRAW_HORIZONTAL_SINGLE ? 0 : 1) + row,x + 2);
			
			for(uint column = 0;column < width;++column){
				uint k = (draw_mode == DRAW_VERTICAL ? column * WRAP_SIZE + row : row * WRAP_SIZE + column);
				
				print_tuple(block,k);
				
				if(k + 1 < len){
					addch(',');
				}
				
				addch(' ');
			}
		}
		
		break;
	}
}

void tuple_set::draw_read(int y,int x,draw draw_mode) const{
	if(draw_mode == DRAW_INVALID){
		return;
	}
	
	// Calculate read draw parameters
	int container_end_y,container_end_x;
	bool bracket;
	
	switch(draw_mode){
	case DRAW_INVALID:
	case DRAW_TUPLE:
		container_end_y = y;
		container_end_x = x + TUPLE_PRINT_WIDTH;
		
		bracket = false;
		
		break;
	case DRAW_HORIZONTAL_SINGLE:
		container_end_y = y;
		container_end_x = x + 2 + len * (TUPLE_PRINT_WIDTH + 2) + 1;
		
		bracket = true;
		
		break;
	case DRAW_HORIZONTAL_MULTI:
	case DRAW_VERTICAL:
		container_end_y = y + 1 + contents_height(draw_mode) + 1;
		container_end_x = x + 1;
		
		bracket = true;
		
		break;
	}
	
	// Draw displays
	switch(state){
	case READ_SET:
		// Draw read in place of contents
		move(y,x);
		
		if(bracket){
			addch('{');
			addch(' ');
		}
		
		print_tuple(buffer,0);
		
		if(bracket){
			addch(' ');
			addch('}');
		}
		
	case READ_IDEMPOTENT:
		// Clear read at the end of the container
		move(container_end_y,container_end_x);
		
		for(uint c = 0;c < 3 + 2 + TUPLE_PRINT_WIDTH + 2;++c){
			addch(' ');
		}
		
		break;
	case READ_ADD:
	case READ_REMOVE:
		// Draw read at the end of the container
		move(container_end_y,container_end_x);
		
		addch(' ');
		addch(state == READ_ADD ? 'U' : '\\');
		addch(' ');
		
		if(bracket){
			addch('{');
			addch(' ');
		}
		
		print_tuple(buffer,0);
		
		if(bracket){
			addch(' ');
			addch('}');
		}
		
		break;
	}
}

// ------------------------------------------------------------ ||
tuple_set::tuple_set(uint INIT_NONVAR_N,uint INIT_N,uint INIT_BLOCK_SIZE,char init_prefix_1,char init_prefix_2,const tuple_set * const * init_supersets,symb *init_buffer,symb *init_block):
	NONVAR_N(INIT_NONVAR_N),N(INIT_N),BLOCK_SIZE(INIT_BLOCK_SIZE),
	WRAP_SIZE(N > 1 ? 8 : 32),TUPLE_PRINT_WIDTH((N > 0 ? (NONVAR_N < N ? NONVAR_N : N - 1) : 0) + N + (N > 1 ? 2 : 0)), // see print_tuple
	
	prefix_1(init_prefix_1),prefix_2(init_prefix_2),
	
	state(READ_IDEMPOTENT),
	
	pos(0),
	supersets(init_supersets),
	buffer(init_buffer),
	
	len(0),
	block(init_block),
	
	prev_height(0),
	redraw_component(true),
	redraw_read(true)
{
	// Nothing
}

void tuple_set::edit(int in){
	if(state == READ_IDEMPOTENT){
		switch(in){
		case 'u':
		case 'U':
			if(BLOCK_SIZE > 1){
				init_read(READ_ADD);
			}
			
			break;
		case '\\':
			if(BLOCK_SIZE > 1){
				init_read(READ_REMOVE);
			}
			
			break;
		case '=':
			if(BLOCK_SIZE == 1){
				init_read(READ_SET);
			}
			
			break;
		case '/':
			if(BLOCK_SIZE == 1){
				on_clear();
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
			
			redraw_read = true;
			
			break;
		case '\t':
		case '\n':
		case '\r':
			if(pos < NONVAR_N){
				break;
			}
			
			switch(state){
			case READ_IDEMPOTENT:
				break;
			case READ_ADD:
				on_add();
				
				break;
			case READ_REMOVE:
				on_remove();
				
				break;
			case READ_SET:
				on_set();
				
				break;
			}
			
			if(in == '\t' && (state == READ_ADD || state == READ_REMOVE)){
				init_read(state);
			}else{
				init_read(READ_IDEMPOTENT);
			}
			
			break;
		case '`':
			init_read(READ_IDEMPOTENT);
			
			break;
		default:
			if(in > (char)in){
				break;
			}
			
			if(pos < N && is_symbol((char)in)){
				symb new_val = symbol((char)in);
				
				if(supersets[pos] == NULL || supersets[pos]->contains(new_val)){
					buffer[pos] = new_val;
					++pos;
					
					redraw_read = true;
				}
			}
			
			break;
		}
	}
}

bool tuple_set::contains(symb val) const{
	if(N != 1){
		// (val) is a 1-tuple
		return false;
	}
	
	for(uint i = 0;i < len;++i){
		if(block[i] == val){
			return true;
		}
	}
	
	return false;
}

int tuple_set::draw(int y) const{
	// Calculate current draw mode
	draw draw_mode;
	
	if(N == 0 || BLOCK_SIZE == 0){
		draw_mode = DRAW_INVALID;
		
	}else if(BLOCK_SIZE == 1){
		draw_mode = DRAW_TUPLE;
		
	}else if(N == 1){
		if(len <= WRAP_SIZE){
			draw_mode = DRAW_HORIZONTAL_SINGLE;
			
		}else{
			draw_mode = DRAW_HORIZONTAL_MULTI;
			
		}
	}else{
		draw_mode = DRAW_VERTICAL;
	}
	
	// Redraw relevant parts
	uint current_height = height();
	
	if(redraw_component){
		// Clear the component's old space
		for(uint r = 0;r < prev_height;++r){
			move(y + r,draw_x);
			clrtoeol();
		}
		
		// Adjust space as needed
		if(prev_height != current_height){
			move(y,draw_x);
			
			if(current_height < prev_height){
				for(uint r = 0;r < (prev_height - current_height);++r){
					deleteln();
				}
				
			}else{
				for(uint r = 0;r < (current_height - prev_height);++r){
					insertln();
				}
			}
		}
		
		// Draw and update parameters
		draw_component(y,draw_x,draw_mode);
		redraw_read = true;
	}
	
	if(redraw_read){
		draw_read(y,draw_x,draw_mode);
	}
	
	// Update redraw data
	prev_height = current_height;
	
	redraw_component = false;
	redraw_read = false;
	
	// Done
	return y + current_height;
}

// ------------------------------------------------------------ ||
