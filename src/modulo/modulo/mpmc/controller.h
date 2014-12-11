#ifndef CONTROLLER_H_0LX27BKO
#define CONTROLLER_H_0LX27BKO

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <inttypes.h>
#include <modulo/mpmc/queue.h>

#if defined(MPMC_USER_TYPES) && defined(MPMC_USER_FN)

#define MPMC_CONTROLLER_DEC(T) \
	typedef _Bool (*MPMC_PRODUCER_TYPE(T))(void * source, T * placeholder); \
	typedef _Bool (*MPMC_CONSUMER_TYPE(T))(void * source, T job);\
	typedef struct mpmc_controller_##T##_struct { \
		void * source; \
		_Bool is_running; \
		atomic_uint_fast64_t producers_running; \
		atomic_bool should_stop; \
		uint64_t producer_count; \
		pthread_t * producer_threads; \
		uint64_t consumer_count; \
		pthread_t * consumer_threads; \
		pthread_t watcher_thread; \
		MPMC_PRODUCER_TYPE(T) * producers; \
		MPMC_CONSUMER_TYPE(T) consumer; \
		MPMC_QUEUE_TYPE(T) queue; \
	} MPMC_TYPE(T); \
	typedef struct _mpmc_data_##T##_struct { \
		uint64_t index; \
		MPMC_TYPE(T) * controller; \
	} _mpmc_data_##T##_t; \
	void MPMC_FN(init, T)(MPMC_TYPE(T) * controller, void * source, \
			uint64_t consumers, uint64_t queue_size); \
	void MPMC_FN(destroy, T)(MPMC_TYPE(T) * controller); \
	_Bool MPMC_FN(add_producer, T)(MPMC_TYPE(T) * controller, \
			MPMC_PRODUCER_TYPE(T) producer); \
	void MPMC_FN(set_consumer, T)(MPMC_TYPE(T) * controller, \
			MPMC_CONSUMER_TYPE(T) consumer); \
	_Bool MPMC_FN(run, T)(MPMC_TYPE(T) * controller); \
	_Bool MPMC_FN(has_producer_running, T)(MPMC_TYPE(T) * controller); \
	void MPMC_FN(stop, T)(MPMC_TYPE(T) * controller); \
	void * MPMC_FN(producer, T)(void * data); \
	void * MPMC_FN(consumer, T)(void * data); \
	void * MPMC_FN(watcher, T)(void * data); \
	void MPMC_FN(wait, T)(MPMC_TYPE(T) * controller);

#define MPMC_CONTROLLER_DEF(T) \
	void MPMC_FN(init, T)(MPMC_TYPE(T) * controller, void * source, \
			uint64_t consumers, uint64_t queue_size) { \
		controller->source = source; \
		controller->is_running = 0; \
		atomic_init(&controller->producers_running, 0); \
		atomic_init(&controller->should_stop, 0); \
		controller->producer_count = 0; \
		controller->producer_threads = NULL; \
		controller->consumer_count = consumers; \
		controller->consumer_threads = malloc(sizeof(pthread_t) * consumers); \
		controller->producers = NULL; \
		controller->consumer = NULL; \
		mpmc_queue_init(&controller->queue, queue_size); \
	}\
	void MPMC_FN(destroy, T)(MPMC_TYPE(T) * controller) { \
		free(controller->consumer_threads); \
		if (controller->producer_threads) free(controller->producer_threads); \
		if (controller->producers) free(controller->producers); \
		mpmc_queue_destroy(&controller->queue); \
	} \
	_Bool MPMC_FN(add_producer, T)(MPMC_TYPE(T) * controller, \
			MPMC_PRODUCER_TYPE(T) producer) { \
		_Bool result; \
		\
		if (controller->producer_count > 0) { \
			controller->producers = realloc(controller->producers, \
				sizeof(MPMC_PRODUCER_TYPE(T)) * ++controller->producer_count); \
		} \
		else { \
			controller->producers = malloc(sizeof(MPMC_PRODUCER_TYPE(T))); \
			controller->producer_count = 1; \
		} \
		\
		result = !(controller->producers == NULL); \
		\
		if (result) { \
			controller->producers[controller->producer_count - 1] = producer; \
		} \
		\
		return result; \
	} \
	void MPMC_FN(set_consumer, T)(MPMC_TYPE(T) * controller, \
			MPMC_CONSUMER_TYPE(T) consumer) { \
		controller->consumer = consumer; \
	} \
	_Bool MPMC_FN(run, T)(MPMC_TYPE(T) * controller) { \
		uint64_t i; \
		_mpmc_data_##T##_t * producer_data; \
		\
		if ((controller->producer_count == 0) || !controller->consumer) { \
			fprintf(stderr, "No consumer/producer defined."); \
			abort(); \
		} \
		\
		if (controller->is_running) return 0; \
		\
		atomic_store(&controller->should_stop, 0); \
		controller->is_running = 1; \
		\
		controller->producer_threads = malloc(sizeof(pthread_t) * \
				controller->producer_count); \
		\
		for (i = 0; i < controller->producer_count; i++) { \
			producer_data = malloc(sizeof(_mpmc_data_##T##_t)); \
			producer_data->index = i; \
			producer_data->controller = controller; \
			atomic_fetch_add(&controller->producers_running, 1); \
			\
			pthread_create(controller->producer_threads + i, NULL, \
				MPMC_FN(producer, T), (void *) producer_data); \
		} \
		\
		for (i = 0; i < controller->consumer_count; i++) { \
			pthread_create(controller->consumer_threads + i, NULL, \
					MPMC_FN(consumer, T), (void *) controller); \
		} \
		\
		pthread_create(&controller->watcher_thread, NULL, \
				MPMC_FN(watcher, T), (void *) controller); \
		\
		return 1; \
	} \
	_Bool MPMC_FN(has_producer_running, T)(MPMC_TYPE(T) * controller) { \
		uint_fast64_t count = atomic_load(&controller->producers_running); \
		\
		return count > 0; \
	} \
	void MPMC_FN(stop, T)(MPMC_TYPE(T) * controller) { \
		atomic_store(&controller->should_stop, 1); \
	} \
	void * MPMC_FN(producer, T)(void * data) { \
		_mpmc_data_##T##_t * _data = (_mpmc_data_##T##_t *) data; \
		uint64_t producer_index = _data->index; \
		MPMC_TYPE(T) * controller = (MPMC_TYPE(T) *) _data->controller; \
		MPMC_PRODUCER_TYPE(T) produce = controller->producers[producer_index]; \
		T job; \
		\
		free(_data); \
		\
		while (produce(controller->source, &job)) { \
			while (!mpmc_queue_push((&controller->queue), job)) { \
				if (atomic_load(&controller->should_stop)) break; \
			} \
			\
			if (atomic_load(&controller->should_stop)) break; \
		} \
		printf("Producer %" PRIu64 " stopping.\n", producer_index); \
		\
		atomic_fetch_sub(&controller->producers_running, 1); \
		\
		return NULL; \
	} \
	void * MPMC_FN(consumer, T)(void * data) { \
		MPMC_TYPE(T) * controller = (MPMC_TYPE(T) *) data; \
		T job; \
		\
		while (!atomic_load(&controller->should_stop)) {\
			if (!mpmc_queue_pop(&controller->queue, &job)) { \
				if (MPMC_FN(has_producer_running, T)(controller)) continue; \
				else break; \
			} \
			atomic_fetch_or(&controller->should_stop, \
					!controller->consumer(controller->source, job)); \
		} \
		printf("Consumer stopping.\n"); \
		\
		return NULL; \
	} \
	void * MPMC_FN(watcher, T)(void * data) { \
		MPMC_TYPE(T) * controller = (MPMC_TYPE(T) *) data; \
		uint64_t i; \
		\
		for (i = 0; i < controller->producer_count; i++) { \
			pthread_join(controller->producer_threads[i], NULL); \
		} \
		\
		for (i = 0; i < controller->consumer_count; i++) { \
			pthread_join(controller->consumer_threads[i], NULL); \
		} \
		\
		controller->is_running = 0;\
		\
		return NULL; \
	} \
	void MPMC_FN(wait, T)(MPMC_TYPE(T) * controller) { \
		pthread_join(controller->watcher_thread, NULL); \
	}

#define MPMC_DECLARE(ALL_TYPES) ALL_TYPES(MPMC_CONTROLLER_DEC)

#define MPMC_DEFINE(ALL_TYPES) \
	MPMC_QUEUE_DEFINE(ALL_TYPES) \
	ALL_TYPES(MPMC_CONTROLLER_DEF)
	

MPMC_DECLARE(MPMC_USER_TYPES)

#define mpmc_controller_init(C, SRC, CONS, QSZ) \
	MPMC_USER_FN(C, init)((C), (SRC), (CONS), (QSZ))

#define mpmc_controller_destroy(C) \
	MPMC_USER_FN(C, destroy)((C))

#define mpmc_controller_add_producer(C, PRODUCER) \
	MPMC_USER_FN(C, add_producer)((C), (PRODUCER))

#define mpmc_controller_set_consumer(C, CONSUMER) \
	MPMC_USER_FN(C, set_consumer)((C), (CONSUMER))

#define mpmc_controller_has_producer_running(C) \
	MPMC_USER_FN(C, has_producer_running)((C))

#define mpmc_controller_stop(C) \
	MPMC_USER_FN(C, stop)((C))

#define mpmc_controller_run(C) \
	MPMC_USER_FN(C, run)((C))

#define mpmc_controller_wait(C) \
	MPMC_USER_FN(C, wait)((C))




#endif

#endif /* end of include guard: CONTROLLER_H_0LX27BKO */

