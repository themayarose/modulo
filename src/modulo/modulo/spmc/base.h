#ifndef QUEUE_BASE_H_FPWONHMT
#define QUEUE_BASE_H_FPWONHMT

#define SPMC_QUEUE_TYPE(T) spmc_queue_##T##_t
#define SPMC_QUEUE_FN(NAME, T) spmc_queue_##NAME##_##T
#define SPMC_TYPE(T) spmc_controller_##T##_t
#define SPMC_FN(NAME, T) spmc_controller_##NAME##_##T

#define SPMC_QUEUE_FN_GEN(T, NAME) SPMC_QUEUE_TYPE(T) *: SPMC_QUEUE_FN(NAME, T)
#define SPMC_FN_GEN(T, NAME) SPMC_TYPE(T) *: SPMC_FN(NAME, T)

#endif /* end of include guard: QUEUE_BASE_H_FPWONHMT */

