#include <stdio.h>
#include <stdlib.h>
#include <modulo/modulo.h>
#include <unistd.h>
#include <time.h>

bool produce(void * source, int * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count == c->total) return false;

	usleep(50000 + rand() % 50000);

	*job_here = c->source[c->count];

	printf("Pushed job: %d\n", *job_here);

	c->count++;

	return true;
}

bool produce_double(void * source, int * job_here) {
	mod_controller_t * c = (mod_controller_t *) source;

	if (c->count_double == c->total) return false;

	usleep(50000 + rand() % 50000);

	*job_here = c->source[c->count_double] + 100;

	printf("Pushed job: %d\n", *job_here);

	c->count_double++;

	return true;
}

bool consume(void * source, int job) {
	printf("Retrieved job: %d\n", job);

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

	for (i = 0; i < c.total; i++) c.source[i] = i;

	mpmc_controller_init(&c.controller, &c, 3, 5);

	mpmc_controller_add_producer(&c.controller, produce);
	mpmc_controller_add_producer(&c.controller, produce_double);
	mpmc_controller_set_consumer(&c.controller, consume);

	mpmc_controller_run(&c.controller);
	mpmc_controller_wait(&c.controller);

	mpmc_controller_destroy(&c.controller);

	free(c.source);


	return EXIT_SUCCESS;
}
