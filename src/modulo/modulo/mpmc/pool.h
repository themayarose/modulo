#ifndef POOL_H_0LAY26LR
#define POOL_H_0LAY26LR

#include <modulo/mpmc/queue.h>

#if defined(MPMC_USER_TYPES) && defined(MPMC_USER_POOL_FN)

#define MPMC_POOL_DEC(T) typedef struct mpmc_controller_##T##_struct { \
	} MPMC_TYPE(T);


#endif
#endif /* end of include guard: POOL_H_0LAY26LR */
