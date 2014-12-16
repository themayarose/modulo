#ifndef POOL_H_WVTELOP9
#define POOL_H_WVTELOP9

#include <stdbool.h>
#include <stdint.h>

#include <modulo/solver/node.h>
#include <modulo/solver/mpmc_types.h>

typedef struct mod_pool_struct {
	mod_node_ptr_t pool;
	MPMC_QUEUE_TYPE(mod_node_ptr_t) available;
} mod_pool_t;

bool mod_pool_init(mod_pool_t * pool, uint64_t size);
void mod_pool_destroy(mod_pool_t * pool);
mod_node_ptr_t mod_pool_alloc(mod_pool_t * pool);
void mod_pool_free(mod_pool_t * pool, mod_node_ptr_t pointer);

#endif /* end of include guard: POOL_H_WVTELOP9 */

