#ifndef QUEUE_H_8ZIEPARB
#define QUEUE_H_8ZIEPARB

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if __clang__
#if !__has_include(<stdatomic.h>)
#include <modulo/stdatomic.h>
#else
#include <stdatomic.h>
#endif
#elif __GNUC__
#include <stdatomic.h>
#endif

#if defined(MPMC_USER_TYPES) && defined(MPMC_USER_QUEUE_FN)

#define MPMC_QUEUE_INIT(T) MPMC_QUEUE_FN(init, T)
#define MPMC_QUEUE_DESTROY(T) MPMC_QUEUE_FN(destroy, T)
#define MPMC_QUEUE_PUSH(T) MPMC_QUEUE_FN(push, T)
#define MPMC_QUEUE_POP(T) MPMC_QUEUE_FN(pop, T)
#define MPMC_QUEUE_EXAUSTED(T) MPMC_QUEUE_FN(exausted, T)

#define MPMC_QUEUE_DEC(T) typedef struct \
	mpmc_queue_##T##_struct { \
			T * buffer; \
			uint64_t size; \
			atomic_uint_fast64_t head; \
			atomic_uint_fast64_t tail; \
			atomic_bool exausted; \
		} MPMC_QUEUE_TYPE(T); \
		_Bool MPMC_QUEUE_INIT(T)(MPMC_QUEUE_TYPE(T) * queue, uint64_t size); \
		void MPMC_QUEUE_DESTROY(T)(MPMC_QUEUE_TYPE(T) * queue); \
		_Bool MPMC_QUEUE_EXAUSTED(T)(MPMC_QUEUE_TYPE(T) * queue); \
		_Bool MPMC_QUEUE_PUSH(T)(MPMC_QUEUE_TYPE(T) * queue, T element); \
		_Bool MPMC_QUEUE_POP(T)(MPMC_QUEUE_TYPE(T) * queue, T * placeholder);

#define MPMC_QUEUE_DEF(T) \
	_Bool MPMC_QUEUE_INIT(T)(MPMC_QUEUE_TYPE(T) * queue, uint64_t size) { \
		queue->size = size; \
		queue->buffer = malloc(sizeof(T) * size); \
		\
		if (!queue->buffer) return 0; \
		\
		memset(queue->buffer, 0, sizeof(T) * size); \
		atomic_init(&queue->head, 1); \
		atomic_init(&queue->tail, 1); \
		atomic_init(&queue->exausted, 0); \
		\
		return 1; \
	} \
	void MPMC_QUEUE_DESTROY(T)(MPMC_QUEUE_TYPE(T) * queue) { \
		free(queue->buffer);\
	} \
	_Bool MPMC_QUEUE_EXAUSTED(T)(MPMC_QUEUE_TYPE(T) * queue) { \
		return atomic_load(&queue->exausted); \
	} \
	_Bool MPMC_QUEUE_PUSH(T)(MPMC_QUEUE_TYPE(T) * queue, T element) {\
		uint_fast64_t target, head, new_tail; \
		_Bool result; \
		\
		result = 0; \
		\
		while (!result) { \
			target = atomic_load(&queue->tail); \
			\
			if (target == 0) continue; \
			\
			head = atomic_load(&queue->head); \
			new_tail = target + 1; \
			\
			if (target == UINT_FAST64_MAX) { \
				atomic_store(&queue->exausted, 1); \
				break; \
			} \
			\
			if (new_tail % queue->size == head % queue->size) break; \
			\
			result = atomic_compare_exchange_weak(&queue->tail, &target, 0); \
		} \
		\
		if (result) { \
			queue->buffer[target % queue->size] = element; \
			atomic_store(&queue->tail, new_tail); \
		} \
		\
		return result; \
	} \
	_Bool MPMC_QUEUE_POP(T)(MPMC_QUEUE_TYPE(T) * queue, \
			T * placeholder) { \
		uint_fast64_t source, tail; \
		_Bool result; \
		\
		result = 0; \
		\
		while (!result) { \
			source = atomic_load(&queue->head); \
			tail = atomic_load(&queue->tail); \
			\
			if (tail == 0) continue; \
			if (source == UINT_FAST64_MAX) break; \
			if (source % queue->size == tail % queue->size) break; \
			\
			*placeholder = queue->buffer[source % queue->size]; \
			\
			result = atomic_compare_exchange_weak(&queue->head, &source, \
					source + 1); \
		} \
		\
		return result; \
	}

#define MPMC_QUEUE_DECLARE(ALL_TYPES) ALL_TYPES(MPMC_QUEUE_DEC)

#define MPMC_QUEUE_DEFINE(ALL_TYPES) ALL_TYPES(MPMC_QUEUE_DEF)

MPMC_QUEUE_DECLARE(MPMC_USER_TYPES)

#define mpmc_queue_init(Q, SIZE) \
	MPMC_USER_QUEUE_FN(Q, init)((Q), (SIZE))

#define mpmc_queue_destroy(Q) \
	MPMC_USER_QUEUE_FN(Q, destroy)((Q))

#define mpmc_queue_exausted(Q) \
	MPMC_USER_QUEUE_FN(Q, exausted)((Q))

#define mpmc_queue_push(Q, ELEM) \
	MPMC_USER_QUEUE_FN(Q, push)((Q), (ELEM))

#define mpmc_queue_pop(Q, PLACEHOLDER) \
	MPMC_USER_QUEUE_FN(Q, pop)((Q), (PLACEHOLDER))

#endif

#endif /* end of include guard: QUEUE_H_8ZIEPARB */

