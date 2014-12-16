#ifndef QUEUE_BASE_H_FPWONHMT
#define QUEUE_BASE_H_FPWONHMT

#define MPMC_QUEUE_TYPE(T) mpmc_queue_##T##_t
#define MPMC_QUEUE_FN(NAME, T) mpmc_queue_##NAME##_##T
#define MPMC_POOL_TYPE(T) mpmc_pool_##T##_t
#define MPMC_POOL_FN(NAME, T) mpmc_pool_##NAME##_##T
#define MPMC_TYPE(T) mpmc_controller_##T##_t
#define MPMC_FN(NAME, T) mpmc_controller_##NAME##_##T

#define MPMC_POOL_PTR(T) mpmc_pool_##T##_ptr

#define MPMC_PRODUCER_TYPE(T) mpmc_producer_##T##_t
#define MPMC_CONSUMER_TYPE(T) mpmc_consumer_##T##_t

#define MPMC_QUEUE_FN_GEN(T, NAME) MPMC_QUEUE_TYPE(T) *: MPMC_QUEUE_FN(NAME, T)
#define MPMC_POOL_FN_GEN(T, NAME) MPMC_POOL_TYPE(T) *: MPMC_POOL_FN(NAME, T)
#define MPMC_FN_GEN(T, NAME) MPMC_TYPE(T) *: MPMC_FN(NAME, T)

#endif /* end of include guard: QUEUE_BASE_H_FPWONHMT */

