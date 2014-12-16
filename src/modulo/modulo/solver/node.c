#include <stdlib.h>
#include <modulo/solver/node.h>


void mod_node_init(mod_node_t * node, uint64_t inicial) {
	node->valor = inicial;
	node->resultado = 0;
}

void mod_node_destroy(mod_node_t * node) {
}
