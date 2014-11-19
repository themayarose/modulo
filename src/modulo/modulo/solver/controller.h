#ifndef CONTROLLER_H_EZOLFSZB
#define CONTROLLER_H_EZOLFSZB

#include <modulo/spmc/base.h>

#define SPMC_USER_TYPES(XX) \
	XX(int)

#define SPMC_USER_FN(T, NAME) _Generic((T), \
		SPMC_FN_GEN(int, NAME) \
	)

#define SPMC_USER_QUEUE_FN(T, NAME) _Generic((T), \
		SPMC_QUEUE_FN_GEN(int, NAME) \
	)


#include <modulo/spmc/controller.h>

typedef struct mod_controller_struct {
	SPMC_TYPE(int) controller;
	int count;
	int total;
	int * source;
} mod_controller_t;

#endif /* end of include guard: CONTROLLER_H_EZOLFSZB */

