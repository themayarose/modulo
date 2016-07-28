#ifndef CONTROLLER_H_EZOLFSZB
#define CONTROLLER_H_EZOLFSZB

#include <modulo/solver/node.h>
#include <modulo/pool/pool.h>
#include <modulo/solver/mpmc_types.h>

typedef struct mod_controller_struct {
	MPMC_TYPE(mod_node_ptr_t) controller;
	mod_pool_t pool;
	uint64_t count_a, count_b;
	uint64_t total;
	uint64_t * source;
	FILE * producers;
	FILE * consumers_a;
	FILE * consumers_b;
} mod_controller_t;

#endif /* end of include guard: CONTROLLER_H_EZOLFSZB */

