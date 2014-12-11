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

bool consume(void * source, int job) {
	printf("%d -> %d\n", job, job * 2);

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
	c.count = 0;
	c.source = malloc(sizeof(int) * c.total);

	for (i = 0; i < c.total; i++) c.source[i] = i;

	spmc_controller_init(&c.controller, &c, 3, 5);

	c.controller.produce = produce;
	c.controller.consume = consume;

	spmc_controller_run(&c.controller);
	spmc_controller_wait(&c.controller);

	spmc_controller_destroy(&c.controller);

	free(c.source);


	return EXIT_SUCCESS;
}
