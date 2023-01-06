#ifndef QUEUE_READ_INCLUDED
	#include "unsigned.h"
	#include "symbol.h"
	
	enum queue_read_mode{
		QUEUE_READ_IDEMPOTENT,
		QUEUE_READ_ADD,
		QUEUE_READ_REMOVE
	};
	
	// ------------------------------------------------------------ ||
	
	#define TRIGGER_DISABLED 0
	#define PREFIX_DISABLED '\0'
	
	struct queue_read_io_config{
		// _update() config -------------------------||
		const bool triggers_chain; // determines whether '\t' re-opens a new read after closing
		                           // the current one
		
		const int trigger_add;     // input that triggers an addition read
		const int trigger_remove;  // input that triggers a removal read
		
		// TRIGGER_DISABLED for either trigger will disable the respective functionality
		
		// _draw() config ---------------------------||
		const char prefix_add;     // character displayed in front of queue contents when adding
		const char prefix_remove;  // character displayed in front of queue contents when removing
		
		// PREFIX_DISABLED for either prefix will cause only queue contents to be drawn in the
		// respective mode, no prefix
		
		const bool bracket;        // enclose queue contents with {}
		const bool paranthesize;   // enclose queue contents with () - if bracket is true, with {()}
	};
	
	#define QUEUE_READ_IO_CONFIG_INIT(TC,TA,TR,PA,PR,B,P) {(TC),(TA),(TR),(PA),(PR),(B),(P)}
	
	// ------------------------------------------------------------ ||
	
	struct set;
	
	struct queue_read{
		// Configuration
		const struct set  * const superset;
		struct queue_read * const subqueue;
		
		const struct queue_read_io_config * const io_conf;
		
		// Execution state
		enum queue_read_mode mode;
		symb value;
	};
	
	#define QUEUE_READ_INIT(SUPERSET,SUBQUEUE,IO_CONF) {(SUPERSET),(SUBQUEUE),(IO_CONF),QUEUE_READ_IDEMPOTENT,SYMBOL_COUNT}
	
	void queue_read_init(struct queue_read *read,enum queue_read_mode mode);
	void queue_read_enq (struct queue_read *read,symb val);
	void queue_read_deq (struct queue_read *read);
	
	const struct set *   queue_read_superset(const struct queue_read *read);
	enum queue_read_mode queue_read_mode    (const struct queue_read *read);
	bool                 queue_read_complete(const struct queue_read *read);
	symb                 queue_read_value   (const struct queue_read *read);
	
	// ------------------------------------------------------------ ||
	// read->io_conf cannot be NULL for these methods
	
	void queue_read_update(struct queue_read *read,int in,bool is_switching,void (*on_submit)(enum queue_read_mode));
	
	int queue_read_draw(const struct queue_read *read);
	int queue_read_nodraw(int y);
	
	#define QUEUE_READ_INCLUDED
#endif