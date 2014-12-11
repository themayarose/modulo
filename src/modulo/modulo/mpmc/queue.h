#ifndef QUEUE_H_8ZIEPARB
#define QUEUE_H_8ZIEPARB

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
#define MPMC_QUEUE_NEXT(Q, C) \
	((C) + 1 < (Q)->buffer + (Q)->size) ? (C) + 1 : (Q)->buffer

#define MPMC_QUEUE_DEC(T) typedef struct \
	mpmc_queue_##T##_struct { \
			T * buffer; \
			uint64_t size; \
			_Atomic(T *) head; \
			_Atomic(T *) tail; \
		} MPMC_QUEUE_TYPE(T); \
		void MPMC_QUEUE_INIT(T)(MPMC_QUEUE_TYPE(T) * queue, uint64_t size); \
		void MPMC_QUEUE_DESTROY(T)(MPMC_QUEUE_TYPE(T) * queue); \
		_Bool MPMC_QUEUE_PUSH(T)(MPMC_QUEUE_TYPE(T) * queue, T element); \
		_Bool MPMC_QUEUE_POP(T)(MPMC_QUEUE_TYPE(T) * queue, T * placeholder);

#define MPMC_QUEUE_DEF(T) \
	void MPMC_QUEUE_INIT(T)(MPMC_QUEUE_TYPE(T) * queue, uint64_t size) { \
		queue->size = size; \
		queue->buffer = malloc(sizeof(T) * size); \
		memset(queue->buffer, 0, sizeof(T) * size); \
		atomic_init(&queue->head, queue->buffer); \
		atomic_init(&queue->tail, queue->buffer); \
	} \
	void MPMC_QUEUE_DESTROY(T)(MPMC_QUEUE_TYPE(T) * queue) { \
		free(queue->buffer);\
	} \
	_Bool MPMC_QUEUE_PUSH(T)(MPMC_QUEUE_TYPE(T) * queue, T element) {\
		T * target, * head, * new_tail; \
		_Bool result; \
		\
		result = 0; \
		\
		while (!result) { \
			target = atomic_load(&queue->tail); \
			head = atomic_load(&queue->head); \
			new_tail = MPMC_QUEUE_NEXT(queue, target); \
		\
			if (new_tail == head) break; \
		\
			result = atomic_compare_exchange_strong(&queue->tail, &target, \
					new_tail); \
		} \
		\
		if (result) *target = element; \
		\
		return result; \
	} \
	_Bool MPMC_QUEUE_POP(T)(MPMC_QUEUE_TYPE(T) * queue, \
			T * placeholder) { \
		T * source, * tail; \
		_Bool result; \
		\
		result = 0; \
		\
		while (!result) { \
			source = atomic_load(&queue->head); \
			tail = atomic_load(&queue->tail); \
			\
			if (source == tail) break; \
			\
			*placeholder = *source; \
			\
			result = atomic_compare_exchange_strong(&queue->head, &source, \
					MPMC_QUEUE_NEXT(queue, source)); \
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

#define mpmc_queue_push(Q, ELEM) \
	MPMC_USER_QUEUE_FN(Q, push)((Q), (ELEM))

#define mpmc_queue_pop(Q, PLACEHOLDER) \
	MPMC_USER_QUEUE_FN(Q, pop)((Q), (PLACEHOLDER))

#endif

#endif /* end of include guard: QUEUE_H_8ZIEPARB */

