#include <stdio.h>
#include <stdlib.h>
#include <modulo/modulo.h>
#include <unistd.h>
#include <time.h>

bool produce(void * source, mod_node_ptr_t * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count == c->total) return false;
	if (mpmc_queue_exausted(&c->controller.queue)) {
		printf("queue exausted.\n");
		return false;
	}

	/* while (!(*job_here = mod_pool_alloc(&c->pool))) { */
	while (!(*job_here = malloc(sizeof(mod_node_t)))) {
		printf("produce has no memory\n");
	}

	mod_node_init(*job_here, c->source[c->count]);

	c->count++;

	fprintf(c->producers, "Job: %" PRIu64 ", %p\n", (*job_here)->valor,
			*job_here);

	return true;
}

bool produce_offset(void * source, mod_node_ptr_t * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count_double == c->total) return false;
	if (mpmc_queue_exausted(&c->controller.queue)) {
		printf("queue exausted.\n");
		return false;
	}

	/* while (!(*job_here = mod_pool_alloc(&c->pool))) { */
	while (!(*job_here = malloc(sizeof(mod_node_t)))) {
		printf("produce offset has no memory\n");
	}

	mod_node_init(*job_here, c->source[c->count_double] + 5000);

	c->count_double++;

	fprintf(c->producers, "Job: %" PRIu64 ", %p\n", (*job_here)->valor,
			*job_here);

	return true;
}


bool consume(void * source, mod_node_ptr_t job) {
	mod_controller_t * c = (mod_controller_t *) source;

	fprintf(c->consumers, "Job: %" PRIu64 ", %p\n", job->valor, job);

	/* mod_pool_free(&c->pool, job); */
	free(job);

	/*
	if (rand() % 100 == 0) {
		printf("STOP RIGHT NOW! THANK YOU VERY MUCH!\n");
		printf("I NEED SOMEBODY WITH A HUMAN TOUCH!\n");
		return false;
	}
	*/

	return true;
}

int main(int argc, char ** argv) {
	mod_controller_t c;
	time_t start, finish;
	long double elapsed;
	int i;

	srand(time(0));

	c.total = 10000000;
	c.count = c.count_double = 0;
	c.source = malloc(sizeof(uint64_t) * c.total);
	c.producers = fopen("producers.txt", "w");
	c.consumers = fopen("consumers.txt", "w");

	for (i = 0; i < c.total; i++) c.source[i] = i;

	mpmc_controller_init( \
		&c.controller, /* controller */
		&c, /* source */
		2, /* consumer count */
		5 /* queue size */
		);

	mod_pool_init(&c.pool, 50);

	mpmc_controller_add_producer(&c.controller, produce);
	mpmc_controller_add_producer(&c.controller, produce_offset);
	mpmc_controller_set_consumer(&c.controller, consume);

	start = time(NULL);
	mpmc_controller_run(&c.controller);
	mpmc_controller_wait(&c.controller);
	finish = time(NULL);

	elapsed = (long double) finish - (long double) start;

	printf("Time: %Lgs\n", elapsed);

	mpmc_controller_destroy(&c.controller);
	mod_pool_destroy(&c.pool);

	free(c.source);

	fclose(c.producers);
	fclose(c.consumers);


	return EXIT_SUCCESS;
}
