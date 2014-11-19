#include <stdlib.h>
#include <modulo/solver/node.h>


mod_node_t * mod_node_init() {
	mod_node_t * node;

	node = malloc(sizeof(mod_node_t *));
	node->next = NULL;

	return node;
}

void mod_node_destroy(mod_node_t * node) {
	free(node);
}
