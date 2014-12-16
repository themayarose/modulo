#ifndef QUEUE_TYPES_H_LI63MQZN
#define QUEUE_TYPES_H_LI63MQZN

#include <modulo/solver/node.h>
#include <modulo/mpmc/base.h>

#define MPMC_USER_TYPES(XX) \
	XX(int) \
	XX(mod_node_t) \
	XX(mod_node_ptr_t)

#define MPMC_USER_QUEUE_FN(T, NAME) _Generic((T), \
		MPMC_QUEUE_FN_GEN(int, NAME), \
		MPMC_QUEUE_FN_GEN(mod_node_t, NAME), \
		MPMC_QUEUE_FN_GEN(mod_node_ptr_t, NAME) \
	)

#define MPMC_USER_POOL_FN(T, NAME) _Generic((T), \
		MPMC_POOL_FN_GEN(int, NAME), \
		MPMC_POOL_FN_GEN(mod_node_t, NAME), \
		MPMC_POOL_FN_GEN(mod_node_ptr_t, NAME) \
	)

#define MPMC_USER_FN(T, NAME) _Generic((T), \
		MPMC_FN_GEN(int, NAME), \
		MPMC_FN_GEN(mod_node_t, NAME), \
		MPMC_FN_GEN(mod_node_ptr_t, NAME) \
	)


#include <modulo/mpmc/controller.h>


#endif /* end of include guard: QUEUE_TYPES_H_LI63MQZN */

