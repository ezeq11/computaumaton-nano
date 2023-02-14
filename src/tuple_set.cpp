
#define WRAP_SIZE

class tuple_set{
private:
	enum read{
		READ_IDEMPOTENT,
		READ_ADD,
		READ_REMOVE,
		READ_SET
	};
	
	enum draw{
		DRAW_INVALID,
		DRAW_TUPLE,
		DRAW_HORIZONTAL_SINGLE,
		DRAW_HORIZONTAL_MULTI,
		DRAW_VERTICAL
	};
	
	// Fields ----------------------------------------------------- ||
	const uint NONVAR_N,N,BLOCK_SIZE;
	const uint TUPLE_PRINT_WIDTH;
	
	const char prefix_1,prefix_2;
	
	// State
	read state;
	
	// Edit data
	uint pos;
	const tuple_set * const * const supersets; // array expected to have at least N elements
	symb * const buffer; // ..................... array expected to have at least N elements
	
	// Set data
	uint len;
	symb * const block; //....................... array expected to have at least N * BLOCK_SIZE elements
	
	// Draw data
	bool redraw_contents;
	draw prev_draw_mode;
	uint prev_len;
	uint prev_height;
	uint contents_redraw_start;
	
	bool redraw_buffer;
	
	// Edit methods ----------------------------------------------- ||
	void init_read(read new_state);
	void update_draw_mode();
	
	void on_add();
	void on_remove();
	void on_set();
	void on_clear();
	
	// Draw methods ----------------------------------------------- ||
	uint contents_height() const;
	void print_tuple(symb *tuples,uint i) const;
	
	void row_iterate_contents_from(int y,int x,uint i,void (tuple_set::*f)(uint,uint,uint) const) const;
	void row_clear(uint row,uint begin_column,uint width) const;
	void row_print(uint row,uint begin_column,uint width) const;
	
	void clear_contents_from(int y,int x,uint i) const;
	void print_contents_from(int y,int x,uint i) const;
	
	void draw_container(int y,int x) const;
	void draw_buffer(int y,int x) const;
	
public:
	tuple_set(uint INIT_NONVAR_N,uint INIT_N,uint INIT_BLOCK_SIZE,char init_prefix_1,char init_prefix_2,const tuple_set *init_supersets,symb *init_buffer,symb *init_block);
	
	void edit(int in);
	bool contains(symb val) const;
};

// Edit methods ----------------------------------------------- ||
void tuple_set::init_read(read new_state){
	state = new_state;
	pos = 0;
	
	memset(buffer,SYMBOL_COUNT,N * sizeof(symb));
}

void tuple_set::remove_if(bool (*tuple_set::remove_tuple)(uint) const){
	// O(n) algorithm: copy only if not deleted
	uint dest_i = 0;
	
	for(uint src_i = 0;src_i < len;++src_i){
		if((this->*remove_tuple)(src_i)){
			// Skip if to be removed
			set_contents_redraw_start(src_i);
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

const tuple_set *tuple_set::containing_superset;
symb tuple_set::contained_val;

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
	
	set_contents_redraw_start(insert_i);
}

void tuple_set::on_remove(){
	remove_if(&tuple_set::tuple_equals_buffer);
}

void tuple_set::on_set(){
	memcpy(block,buffer,N * sizeof(symb));
	len = 1;
	
	set_contents_redraw_start(0);
}

void tuple_set::on_clear(){
	len = 0;
	
	set_contents_redraw_start(0);
}

// Draw methods ----------------------------------------------- ||
uint tuple_set::contents_height(uint acting_len) const{
	switch(draw_mode){
	case DRAW_INVALID:
		return 0;
		
	case DRAW_TUPLE:
	case DRAW_HORIZONTAL_SINGLE:
		return 1;
		
	case DRAW_HORIZONTAL_MULTI:
		return (acting_len / WRAP_SIZE) + (acting_len % WRAP_SIZE > 0 ? 1 : 0);
		
	case DRAW_VERTICAL:
		return (acting_len < WRAP_SIZE ? acting_len : WRAP_SIZE);
	}
	
	return 0;
}

void tuple_set::print_tuple(symb *tuples,uint i) const{
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

void tuple_set::row_iterate_contents_from(int y,int x,uint i,uint acting_len,void (tuple_set::*row_op)(uint,uint,uint) const) const{
	if(i >= acting_len){
		return;
	}
	
	switch(draw_mode){
	case DRAW_INVALID:
		
		break;
	case DRAW_TUPLE:
		if(i != 0){
			break;
		}
		
		move(y,x);
		(this->*row_op)(0,0,1);
		
		break;
	case DRAW_HORIZONTAL_SINGLE:
	case DRAW_HORIZONTAL_MULTI:
		uint height = contents_height(acting_len);
		uint first_row = i / WRAP_SIZE;
		
		for(uint row = first_row;row < height;++row){
			uint width        = (row + 1 == height ? acting_len % WRAP_SIZE : WRAP_SIZE);
			uint begin_column = (row == first_row ? i % WRAP_SIZE : 0);
			
			move(y + (draw_mode == DRAW_HORIZONTAL_MULTI ? 1 : 0) + row,x + 2 + begin_column * (TUPLE_PRINT_WIDTH + 2));
			
			(this->*row_op)(row,begin_column,width);
		}
		
		break;
	case DRAW_VERTICAL:
		uint height = contents_height(acting_len);
		
		for(uint row = 0;row < height;++row){
			uint width        = (acting_len / WRAP_SIZE) + (row < (acting_len % WRAP_SIZE) ? 1 : 0);
			uint begin_column = (i          / WRAP_SIZE) + (row < (i          % WRAP_SIZE) ? 1 : 0);
			
			move(y + 1 + row,x + 2 + begin_column * (TUPLE_PRINT_WIDTH + 2));
			
			(this->*row_op)(row,begin_column,width);
		}
		
		break;
	}
}

void tuple_set::row_clear(uint row,uint begin_column,uint width) const{
	clrtoeol();
}

void tuple_set::row_print(uint row,uint begin_column,uint width) const{
	for(uint column = begin_column;column < width;++column){
		uint k = (draw_mode == DRAW_VERTICAL ? column * WRAP_SIZE + row : row * WRAP_SIZE + column);
		
		print_tuple(block,k);
		
		if(k + 1 < len){
			addch(',');
		}
		
		addch(' ');
	}
}

void tuple_set::clear_contents_from(int y,int x,uint i,uint acting_len) const{
	row_iterate_contents_from(y,x,i,acting_len,&tuple_set::row_clear);
}

void tuple_set::print_contents_from(int y,int x,uint i,uint acting_len) const{
	row_iterate_contents_from(y,x,i,acting_len,&tuple_set::row_print);
}

void tuple_set::draw_container(int y,int x) const{
	if(draw_mode == DRAW_INVALID){
		return;
	}
	
	switch(draw_mode){
	case DRAW_INVALID:
		
		break;
	case DRAW_TUPLE:
		move(y,x + TUPLE_PRINT_WIDTH);
		
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
		
		move(y + 1 + contents_height(len) + 1,x);
		addch('}');
		
		break;
	}
	
	// Operation container
	switch(state){
	case READ_IDEMPOTENT:
	case READ_SET:
		
		break;
	case READ_ADD:
	case READ_REMOVE:
		addch(' ');
		addch(state == READ_ADD ? 'U' : '\\');
		addch(' ');
		
		switch(draw_mode){
		case DRAW_INVALID:
		case DRAW_TUPLE:
			
			break;
		case DRAW_HORIZONTAL_SINGLE:
		case DRAW_HORIZONTAL_MULTI:
		case DRAW_VERTICAL:
			addch('{');
			addch(' ');
			
			for(uint c = 0;c < TUPLE_PRINT_WIDTH;++c){
				addch(' ');
			}
			
			addch(' ');
			addch('}');
			
			break;
		}
		
		break;
	}
}

void tuple_set::draw_buffer(int y,int x) const{
	switch(state){
	case READ_IDEMPOTENT:
		
		break;
	case READ_ADD:
	case READ_REMOVE:
		switch(draw_mode){
		case DRAW_INVALID:
			
			break;
		case DRAW_TUPLE:
			move(y,x + TUPLE_PRINT_WIDTH + 3);
			print_tuple(buffer,0);
			
			break;
		case DRAW_HORIZONTAL_SINGLE:
			move(y,x + 2 + len * (TUPLE_PRINT_WIDTH + 2) + 1 + 3 + 2);
			print_tuple(buffer,0);
			
			break;
		case DRAW_HORIZONTAL_MULTI:
		case DRAW_VERTICAL:
			move(y + 1 + contents_height(len) + 1,x + 1 + 3 + 2);
			print_tuple(buffer,0);
			
			break;
		}
		
		break;
	case READ_SET:
		switch(draw_mode){
		case DRAW_INVALID:
			
			break;
		case DRAW_TUPLE:
			move(y,x);
			print_tuple(buffer,0);
			
			break;
		case DRAW_HORIZONTAL_SINGLE:
			move(y,x + 2);
			print_tuple(buffer,0);
			
			break;
		case DRAW_HORIZONTAL_MULTI:
		case DRAW_VERTICAL:
			move(y + 1,x + 2);
			print_tuple(buffer,0);
			
			break;
		}
		
		break;
	}
}

void tuple_set::terminate_draw(draw draw_mode){
	redraw_contents = false;
	prev_draw_mode = draw_mode;
	prev_len = len;
	prev_height = height();
	contents_redraw_start = len;
	
	redraw_buffer = false;
}

void tuple_set::set_contents_redraw_start(uint new_contents_redraw_start){
	if(new_contents_redraw_start < contents_redraw_start){
		contents_redraw_start = new_contents_redraw_start;
	}
	
	redraw_contents = true;
}

// ------------------------------------------------------------ ||
tuple_set::tuple_set(uint INIT_NONVAR_N,uint INIT_N,uint INIT_BLOCK_SIZE,char init_prefix_1,char init_prefix_2,const tuple_set *init_supersets,symb *init_buffer,symb *init_block):
	NONVAR_N(INIT_NONVAR_N),N(INIT_N),BLOCK_SIZE(INIT_BLOCK_SIZE),
	TUPLE_PRINT_WIDTH((N > 0 ? (NONVAR_N < N ? NONVAR_N : N - 1) : 0) + N + (N > 1 ? 2 : 0)), // see print_tuple
	
	prefix_1(init_prefix_1),prefix_2(init_prefix_2),
	
	state(READ_IDEMPOTENT),
	
	pos(0),
	supersets(init_supersets),
	buffer(init_buffer),
	
	len(0),
	block(init_block)
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
			
			redraw_buffer = true;
			
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
				state = READ_IDEMPOTENT;
			}
			
			break;
		case '`':
			state = READ_IDEMPOTENT;
			
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
					
					redraw_buffer = true;
				}
			}
			
			break;
		}
	}
}

void tuple_set::init_draw_frame(int y,int x) const{
	draw_container(y,x);
	
	terminate_draw();
}

void tuple_set::init_draw_fill(int y,int x) const{
	print_contents_from(y,x,0);
	draw_buffer(y,x);
	
	terminate_draw();
}

void tuple_set::re_draw(int y) const{
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
	
	// Clear relevant screen space
	if(prev_draw_mode != draw_mode){
		// Change of draw mode: clear entirety
		for(uint r = 0;r < prev_height;++r){
			move(y + r,draw_x);
			clrtoeol();
		}
		
	}else{
		// Same draw mode: clear only relevant portions
		switch(prev_draw_mode){
		case DRAW_INVALID:
			
			break;
		case DRAW_TUPLE:
			if(redraw_contents && redraw_container){
				move(y,draw_x);
				clrtoeol();
				
				redraw_buffer = true;
				
			}else if(redraw_contents){
				move(y,draw_x);
				
				for(uint c = 0;c < TUPLE_PRINT_WIDTH;++c){
					addch(' ');
				}
				
			}else if(redraw_container){
				move(y,draw_x + TUPLE_PRINT_WIDTH);
				clrtoeol();
				
				redraw_buffer = true;
			}
			
			break;
		case DRAW_HORIZONTAL_SINGLE:
			if(redraw_container){
				move(y,draw_x);
				clrtoeol();
				
				set_contents_redraw_start(0);
				redraw_buffer = true;
				
			}else if(redraw_contents){
				clear_contents_from(y,draw_x,contents_redraw_start,prev_len);
				
				redraw_container = true;
				redraw_buffer = true;
				
			}
			
			break;
		case DRAW_HORIZONTAL_MULTI:
		case DRAW_VERTICAL:
			if(redraw_container){
				move(y,draw_x);
				clrtoeol();
				
				move(y + 1 + contents_height(prev_len) + 1,draw_x);
				clrtoeol();
				
				redraw_buffer = true;
			}
			
			if(redraw_contents){
				clear_contents_from(y,draw_x,contents_redraw_start,prev_len);
			}
			
			break;
		}
	}
	
	// Adjust space as needed
	uint current_height = height();
	
	if(prev_height != current_height){
		switch(draw_mode){
		case DRAW_INVALID:
		case DRAW_TUPLE:
		case DRAW_HORIZONTAL_SINGLE:
			move(y + 1,draw_x);
			
			break;
		case DRAW_HORIZONTAL_MULTI:
		case DRAW_VERTICAL:
			move(y + 1 + contents_height(current_height < prev_height ? len : prev_len),draw_x);
			
			break;
		}
		
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
	
	// Redraw appropriately
	if(prev_draw_mode != draw_mode){
		init_draw_frame(y,draw_x);
		init_draw_fill(y,draw_x);
		
	}else{
		if(redraw_container){
			draw_container(y,draw_x);
		}
		
		if(redraw_contents){
			print_contents_from(y,draw_x,contents_redraw_start,len);
		}
		
		if(redraw_buffer){
			draw_buffer(y,draw_x);
		}
	}
	
	terminate_draw();
}

uint tuple_set::height() const{
	switch(draw_mode){
	case DRAW_HORIZONTAL_MULTI:
	case DRAW_VERTICAL:
		
		return 1 + contents_height(len) + 1 + 2;
	case DRAW_INVALID:
	case DRAW_TUPLE:
	case DRAW_HORIZONTAL_SINGLE:
		
		break;
	}
	
	return 2;
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

// ------------------------------------------------------------ ||
