#include <stddef.h>

#include "product.h"
#include "bit_array.h"

const struct queue_read_io_config PRODUCT_READ_CONFIG = QUEUE_READ_IO_CONFIG_INIT(1,'u','\\','U','\\',1,1);

// ------------------------------------------------------------ ||

symb addition_element;
bool addition_index_walk;
uint addition_index;

static void product_read_add_locate_walk(uint i,symb val){
	if(!addition_index_walk){
		return;
	}
	
	if(i < addition_index){
		return;
	}
	
	if(val < addition_element){
		addition_index = i + 1;
	}else{
		addition_index_walk = 0;
	}
}

static void product_read_add_locate(struct product *p){
	if(p == NULL){
		return;
	}
	
	addition_element = queue_read_value(&(p->read));
	addition_index_walk = 1;
	symb_list_forall(&(p->list),&product_read_add_locate_walk);
	
	product_read_add_locate(p->subproduct);
}

static void product_read_add_insert(struct product *p,uint i){
	if(p == NULL){
		return;
	}
	
	symb_list_insert(&(p->list),i,queue_read_value(&(p->read)));
	
	product_read_add_insert(p->subproduct,i);
}

static void product_read_add(struct product *p){
	addition_index = 0;
	product_read_add_locate(p);
	
	product_read_add_insert(p,addition_index);
}

// ------------------------------------------------------------ ||

symb val_to_mark;

BIT_ARRAY_DECL(SYMB_LIST_BLOCK_LEN)
bit_array(SYMB_LIST_BLOCK_LEN) marked_elements;

static void product_marked_clear(){
	bit_array_clear(marked_elements,SYMB_LIST_BLOCK_LEN);
}

static void product_mark_eq_walk(uint i,symb val){
	if(val == val_to_mark){
		bit_array_add(marked_elements,i);
	}
}

static void product_mark_neq_walk(uint i,symb val){
	if(val != val_to_mark){
		bit_array_add(marked_elements,i);
	}
}

static bool product_is_marked_walk(uint i,symb val){
	return bit_array_get(marked_elements,i);
}

static bool product_isnt_marked_walk(uint i,symb val){
	return !bit_array_get(marked_elements,i);
}

// ------------------------------------------------------------ ||

static void product_read_remove_mark(struct product *p){
	if(p == NULL){
		return;
	}
	
	val_to_mark = queue_read_value(&(p->read));
	symb_list_forall(&(p->list),&product_mark_neq_walk);
	
	product_read_remove_mark(p->subproduct);
}

static void product_read_remove_erase(struct product *p){
	if(p == NULL){
		return;
	}
	
	symb_list_removeif(&(p->list),&product_isnt_marked_walk);
	
	product_read_remove_erase(p->subproduct);
}

static void product_read_remove(struct product *p){
	product_marked_clear();
	product_read_remove_mark(p);
	
	product_read_remove_erase(p);
}

// ------------------------------------------------------------ ||

static void product_remove_referencing_mark(struct product *p,struct set *s,symb val){
	if(p == NULL){
		return;
	}
	
	if(p->superset == s){
		val_to_mark = val;
		symb_list_forall(&(p->list),&product_mark_eq_walk);
	}
	
	product_remove_referencing_mark(p->subproduct,s,val);
}

static void product_remove_referencing_erase(struct product *p){
	if(p == NULL){
		return;
	}
	
	symb_list_removeif(&(p->list),&product_is_marked_walk);
	
	product_remove_referencing_erase(p->subproduct);
}

void product_remove_referencing(struct product *p,struct set *s,symb val){
	product_marked_clear();
	product_remove_referencing_mark(p,s,val);
	
	product_remove_referencing_erase(p);
}

// ------------------------------------------------------------ ||

static uint product_size(struct product *p){
	if(p == NULL){
		return 0;
	}
	
	uint this_size = symb_list_len(&(p->list));
	uint sub_size = product_size(p->subproduct);
	
	return this_size > sub_size ? this_size : sub_size;
}

static void product_fortuple(struct product *p,uint i,void (*f)(symb,bool)){
	if(p == NULL){
		return;
	}
	
	f(symb_list_get(&(p->list),i),p->subproduct == NULL);
	
	product_fortuple(p->subproduct,i,f);
}

// ------------------------------------------------------------ ||

struct product *product_current;

static void product_read_on_submit(enum queue_read_mode mode){
	switch(mode){
	case QUEUE_READ_IDEMPOTENT:
		// null
		
		break;
	case QUEUE_READ_ADD:
		product_read_add(product_current);
		
		break;
	case QUEUE_READ_REMOVE:
		product_read_remove(product_current);
		
		break;
	}
}

void product_update(struct product *p,int in,bool is_switching){
	product_current = p;
	queue_read_update(&(p->read),in,is_switching,&product_read_on_submit);
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#include <curses.h>
#else
	#include <ncurses.h>
#endif

static void draw_tuple_member(symb val,bool is_tuple_end){
	addch(ascii(val));
	
	if(!is_tuple_end){
		addch(',');
	}
}

void product_draw(int y,int x,struct product *p,uint max_rows){
	uint prod_size = product_size(p);
	uint total_rows = prod_size > max_rows ? max_rows : prod_size;
	
	/*
	  total_rows = 3
	  prod_size = 7
	  
	  row  0: p_0 p_3 p_6
	  row  1: p_1 p_4
	  row  2: p_2 p_5
	*/
	
	move(y,x);
	addch('{');
	
	for(uint row = 0;row < total_rows;++row){
		uint row_width = (prod_size / total_rows) + (row < (prod_size % total_rows));
		
		move(y + 1 + row,x);
		addch(' ');
		addch(' ');
		
		for(uint column = 0;column < row_width;++column){
			uint i = row + total_rows * column;
			
			addch('(');
			product_fortuple(p,i,&draw_tuple_member);
			addch(')');
			
			if(i + 1 < prod_size){
				addch(',');
				addch(' ');
			}
		}
	}
	
	move(y + 1 + total_rows + 1,x);
	addch('}');
	
	queue_read_draw(&(p->read));
}