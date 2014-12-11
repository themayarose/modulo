#ifndef CONTROLLER_H_EZOLFSZB
#define CONTROLLER_H_EZOLFSZB

#include <modulo/solver/mpmc_types.h>

typedef struct mod_controller_struct {
	MPMC_TYPE(int) controller;
	int count, count_double;
	int total;
	int * source;
	FILE * producers;
	FILE * consumers;
} mod_controller_t;

#endif /* end of include guard: CONTROLLER_H_EZOLFSZB */

