#ifndef NODE_H_4SUFBAKA
#define NODE_H_4SUFBAKA

#include <pthread.h>

typedef struct mod_node_struct {
	struct mod_node_struct * next;
} mod_node_t;

mod_node_t * mod_node_init();
void mod_node_destroy(mod_node_t * node);

#endif /* end of include guard: NODE_H_4SUFBAKA */

