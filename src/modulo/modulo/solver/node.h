#ifndef NODE_H_4SUFBAKA
#define NODE_H_4SUFBAKA

#include <pthread.h>
#include <stdint.h>

typedef enum tag_struct { TAG_A, TAG_B } tag_t;


typedef struct mod_node_struct {
    tag_t tag;
    uint64_t valor;
    uint64_t resultado;
} mod_node_t;

typedef mod_node_t * mod_node_ptr_t;

void mod_node_init(mod_node_t * node, uint64_t inicial, tag_t tag);
void mod_node_destroy(mod_node_t * node);

#endif /* end of include guard: NODE_H_4SUFBAKA */

