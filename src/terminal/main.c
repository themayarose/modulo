#include <stdio.h>
#include <stdlib.h>
#include <modulo/modulo.h>
#include <unistd.h>
#include <time.h>

bool produce(void * source, int * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count == c->total) return false;

	usleep(100000 + rand() % 50000);

	*job_here = c->source[c->count];

	c->count++;

	fprintf(c->producers, "Job: %d\n", *job_here);

	return true;
}

bool produce_double(void * source, int * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count_double == c->total) return false;

	usleep(100000 + rand() % 50000);

	*job_here = c->source[c->count_double] + 100;

	c->count_double++;

	fprintf(c->producers, "Job: %d\n", *job_here);

	return true;
}

bool consume(void * source, int job) {
	mod_controller_t * c = (mod_controller_t *) source;

	fprintf(c->consumers, "Job: %d\n", job);

	usleep(250000 + rand() % 100000);

	if (rand() % 100 == 0) {
		printf("STOP RIGHT NOW! THANK YOU VERY MUCH!\n");
		printf("I NEED SOMEBODY WITH A HUMAN TOUCH!\n");
		return false;
	}

	return true;
}

int main(int argc, char ** argv) {
	mod_controller_t c;
	int i;

	srand(time(0));

	c.total = 100;
	c.count = c.count_double = 0;
	c.source = malloc(sizeof(int) * c.total);
	c.producers = fopen("producers.txt", "w");
	c.consumers = fopen("consumers.txt", "w");

	for (i = 0; i < c.total; i++) c.source[i] = i;

	mpmc_controller_init(&c.controller, &c, 3, 5);

	mpmc_controller_add_producer(&c.controller, produce);
	mpmc_controller_add_producer(&c.controller, produce_double);
	mpmc_controller_set_consumer(&c.controller, consume);

	mpmc_controller_run(&c.controller);
	mpmc_controller_wait(&c.controller);

	mpmc_controller_destroy(&c.controller);

	free(c.source);

	fclose(c.producers);
	fclose(c.consumers);


	return EXIT_SUCCESS;
}
