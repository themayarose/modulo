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

#if defined(SPMC_USER_TYPES) && defined(SPMC_USER_QUEUE_FN)

#define SPMC_QUEUE_INIT(T) SPMC_QUEUE_FN(init, T)
#define SPMC_QUEUE_DESTROY(T) SPMC_QUEUE_FN(destroy, T)
#define SPMC_QUEUE_PUSH(T) SPMC_QUEUE_FN(push, T)
#define SPMC_QUEUE_POP(T) SPMC_QUEUE_FN(pop, T)
#define SPMC_QUEUE_NEXT(Q, C) \
	((C) + 1 < (Q)->buffer + (Q)->size) ? (C) + 1 : (Q)->buffer

#define SPMC_QUEUE_DEC(T) typedef struct \
	spmc_queue_##T##_struct { \
			T * buffer; \
			uint64_t size; \
			_Atomic(T *) head; \
			_Atomic(T *) tail; \
		} SPMC_QUEUE_TYPE(T); \
		void SPMC_QUEUE_INIT(T)(SPMC_QUEUE_TYPE(T) * queue, uint64_t size); \
		void SPMC_QUEUE_DESTROY(T)(SPMC_QUEUE_TYPE(T) * queue); \
		_Bool SPMC_QUEUE_PUSH(T)(SPMC_QUEUE_TYPE(T) * queue, T element); \
		_Bool SPMC_QUEUE_POP(T)(SPMC_QUEUE_TYPE(T) * queue, T * placeholder);

#define SPMC_QUEUE_DEF(T) \
	void SPMC_QUEUE_INIT(T)(SPMC_QUEUE_TYPE(T) * queue, uint64_t size) { \
		queue->size = size; \
		queue->buffer = malloc(sizeof(T) * size); \
		memset(queue->buffer, 0, sizeof(T) * size); \
		atomic_init(&queue->head, queue->buffer); \
		atomic_init(&queue->tail, queue->buffer); \
	} \
	void SPMC_QUEUE_DESTROY(T)(SPMC_QUEUE_TYPE(T) * queue) { \
		free(queue->buffer);\
	} \
	_Bool SPMC_QUEUE_PUSH(T)(SPMC_QUEUE_TYPE(T) * queue, T element) {\
		T * prev_tail, * new_tail;\
		_Bool success; \
		\
		prev_tail = atomic_load(&queue->tail); \
		new_tail = SPMC_QUEUE_NEXT(queue, prev_tail); \
		\
		if (new_tail != atomic_load(&queue->head)) {\
			*prev_tail = element; \
			atomic_store(&queue->tail, new_tail); \
			success = 1; \
		} \
		else { \
			atomic_store(&queue->tail, prev_tail); \
			success = 0; \
		} \
		\
		return success; \
	} \
	_Bool SPMC_QUEUE_POP(T)(SPMC_QUEUE_TYPE(T) * queue, \
			T * placeholder) { \
		T * tmp_head, * tail; \
		_Bool result; \
		volatile _Atomic(T *) * head;\
		\
		head = &queue->head; \
		result = 0; \
		\
		while (!result) { \
			tmp_head = atomic_load(head); \
			tail = atomic_load(&queue->tail); \
			\
			if (tmp_head == tail) break; \
			\
			*placeholder = *tmp_head; \
			\
			result = atomic_compare_exchange_strong(head, &tmp_head, \
					SPMC_QUEUE_NEXT(queue, tmp_head)); \
		} \
		\
		return result; \
	}

#define SPMC_QUEUE_DECLARE(ALL_TYPES) ALL_TYPES(SPMC_QUEUE_DEC)

#define SPMC_QUEUE_DEFINE(ALL_TYPES) ALL_TYPES(SPMC_QUEUE_DEF)

SPMC_QUEUE_DECLARE(SPMC_USER_TYPES)

#define spmc_queue_init(Q, SIZE) \
	SPMC_USER_QUEUE_FN(Q, init)((Q), (SIZE))

#define spmc_queue_destroy(Q) \
	SPMC_USER_QUEUE_FN(Q, destroy)((Q))

#define spmc_queue_push(Q, ELEM) \
	SPMC_USER_QUEUE_FN(Q, push)((Q), (ELEM))

#define spmc_queue_pop(Q, PLACEHOLDER) \
	SPMC_USER_QUEUE_FN(Q, pop)((Q), (PLACEHOLDER))

#endif

#endif /* end of include guard: QUEUE_H_8ZIEPARB */

