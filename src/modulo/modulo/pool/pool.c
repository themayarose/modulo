#include <modulo/pool/pool.h>


bool mod_pool_init(mod_pool_t * pool, uint64_t size) {
	uint64_t i;

	pool->pool = malloc(sizeof(mod_node_t) * size);

	if (!pool->pool) return false;

	if (!mpmc_queue_init(&pool->available, size)) {
		free(pool->pool);
		return false;
	}

	for (i = 0; i < size; i++) {
		mpmc_queue_push(&pool->available, pool->pool + i);
	}

	return false;
}

void mod_pool_destroy(mod_pool_t * pool) {
	free(pool->pool);
	mpmc_queue_destroy(&pool->available);
}

mod_node_ptr_t mod_pool_alloc(mod_pool_t * pool) {
	mod_node_ptr_t pointer;
	
	return (mpmc_queue_pop(&pool->available, &pointer)) ? pointer : NULL;
}

void mod_pool_free(mod_pool_t * pool, mod_node_ptr_t pointer) {
	mpmc_queue_push(&pool->available, pointer);
}

