#ifndef CONTROLLER_H_0LX27BKO
#define CONTROLLER_H_0LX27BKO

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <modulo/spmc/queue.h>

#if defined(SPMC_USER_TYPES) && defined(SPMC_USER_FN)

#define SPMC_CONTROLLER_DEC(T) typedef struct spmc_controller_##T##_struct { \
		void * source; \
		_Bool is_running; \
		_Bool producer_running; \
		atomic_bool should_stop; \
		pthread_t producer_thread; \
		pthread_t watcher_thread; \
		uint64_t consumer_count; \
		pthread_t * consumer_threads; \
		_Bool (*produce)(void * source, T * placeholder); \
		_Bool (*consume)(void * source, T job); \
		SPMC_QUEUE_TYPE(T) queue; \
	} SPMC_TYPE(T); \
	void SPMC_FN(init, T)(SPMC_TYPE(T) * controller, void * source, \
			uint64_t consumers, uint64_t queue_size); \
	void SPMC_FN(destroy, T)(SPMC_TYPE(T) * controller); \
	_Bool SPMC_FN(run, T)(SPMC_TYPE(T) * controller); \
	void SPMC_FN(stop, T)(SPMC_TYPE(T) * controller); \
	void * SPMC_FN(producer, T)(void * data); \
	void * SPMC_FN(consumer, T)(void * data); \
	void * SPMC_FN(watcher, T)(void * data); \
	void SPMC_FN(wait, T)(SPMC_TYPE(T) * controller);

#define SPMC_CONTROLLER_DEF(T) \
	void SPMC_FN(init, T)(SPMC_TYPE(T) * controller, void * source, \
			uint64_t consumers, uint64_t queue_size) { \
		controller->source = source; \
		controller->is_running = 0; \
		controller->producer_running = 0; \
		atomic_init(&controller->should_stop, 0); \
		controller->consumer_count = consumers; \
		controller->consumer_threads = malloc(sizeof(pthread_t) * consumers); \
		controller->produce = NULL; \
		controller->consume = NULL; \
		spmc_queue_init(&controller->queue, queue_size); \
	}\
	void SPMC_FN(destroy, T)(SPMC_TYPE(T) * controller) { \
		free(controller->consumer_threads); \
		spmc_queue_destroy(&controller->queue); \
	} \
	_Bool SPMC_FN(run, T)(SPMC_TYPE(T) * controller) { \
		uint64_t i; \
		\
		if (!controller->produce || !controller->consume) { \
			fprintf(stderr, "No consumer/producer defined."); \
			abort(); \
		} \
		\
		if (controller->is_running) return 0; \
		\
		atomic_exchange(&controller->should_stop, 0); \
		controller->is_running = 1; \
		controller->producer_running = 1; \
		\
		pthread_create(&controller->producer_thread, NULL, \
				SPMC_FN(producer, T), (void *) controller); \
		\
		for (i = 0; i < controller->consumer_count; i++) { \
			pthread_create(controller->consumer_threads + i, NULL, \
					SPMC_FN(consumer, T), (void *) controller); \
		} \
		\
		pthread_create(&controller->watcher_thread, NULL, \
				SPMC_FN(watcher, T), (void *) controller); \
		\
		return 1; \
	} \
	void SPMC_FN(stop, T)(SPMC_TYPE(T) * controller) { \
		atomic_store(&controller->should_stop, 1); \
	} \
	void * SPMC_FN(producer, T)(void * data) { \
		SPMC_TYPE(T) * controller = (SPMC_TYPE(T) *) data; \
		T job; \
		\
		while (controller->produce(controller->source, &job)) { \
			while (!spmc_queue_push((&controller->queue), job)) { \
				if (atomic_load(&controller->should_stop)) break; \
			} \
			\
			if (atomic_load(&controller->should_stop)) break; \
		} \
		printf("Producer stopping.\n"); \
		\
		controller->producer_running = 0; \
		\
		return NULL; \
	} \
	void * SPMC_FN(consumer, T)(void * data) { \
		SPMC_TYPE(T) * controller = (SPMC_TYPE(T) *) data; \
		T job; \
		\
		while (!atomic_load(&controller->should_stop)) {\
			if (!spmc_queue_pop(&controller->queue, &job)) { \
				if (controller->producer_running) continue; \
				else break; \
			} \
			atomic_fetch_or(&controller->should_stop, \
					!controller->consume(controller->source, job)); \
		} \
		printf("Consumer stopping.\n"); \
		\
		return NULL; \
	} \
	void * SPMC_FN(watcher, T)(void * data) { \
		SPMC_TYPE(T) * controller = (SPMC_TYPE(T) *) data; \
		uint64_t i; \
		\
		pthread_join(controller->producer_thread, NULL); \
		\
		for (i = 0; i < controller->consumer_count; i++) { \
			pthread_join(controller->consumer_threads[i], NULL); \
		} \
		\
		controller->is_running = 0;\
		\
		return NULL; \
	} \
	void SPMC_FN(wait, T)(SPMC_TYPE(T) * controller) { \
		pthread_join(controller->watcher_thread, NULL); \
	}

#define SPMC_DECLARE(ALL_TYPES) ALL_TYPES(SPMC_CONTROLLER_DEC)

#define SPMC_DEFINE(ALL_TYPES) \
	SPMC_QUEUE_DEFINE(ALL_TYPES) \
	ALL_TYPES(SPMC_CONTROLLER_DEF)
	

SPMC_DECLARE(SPMC_USER_TYPES)

#define spmc_controller_init(C, SRC, CONS, QSZ) \
	SPMC_USER_FN(C, init)((C), (SRC), (CONS), (QSZ))

#define spmc_controller_destroy(C) \
	SPMC_USER_FN(C, destroy)((C))

#define spmc_controller_stop(C) \
	SPMC_USER_FN(C, stop)((C))

#define spmc_controller_run(C) \
	SPMC_USER_FN(C, run)((C))

#define spmc_controller_wait(C) \
	SPMC_USER_FN(C, wait)((C))




#endif

#endif /* end of include guard: CONTROLLER_H_0LX27BKO */

