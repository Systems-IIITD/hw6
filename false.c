#define _GNU_SOURCE

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CACHE_LINE_SIZE 64
#define MAX_THREADS 2
#define ITERATIONS 1000000000
#define NUM_RANDOMS 128

struct fshare {
	volatile int arr[MAX_THREADS];
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct fshare gvar;
int random_arr[NUM_RANDOMS];

static void *do_compute(void *arg)
{
	int tid = *(int*)arg;
	volatile int *update_loc;
	int i;

	assert(tid < MAX_THREADS);
	update_loc = &gvar.arr[tid];

	for (i = 0; i < ITERATIONS; i++) {
		update_loc[0] += random_arr[i % NUM_RANDOMS];
	}
}


int main()
{
	pthread_t t[MAX_THREADS];
	int arr[MAX_THREADS];
	int i;

	memset((void*)&gvar, 0, sizeof(gvar));

	for (i = 0; i < NUM_RANDOMS; i++) {
		random_arr[i] = rand() % NUM_RANDOMS;
	}

	for (i = 0; i < MAX_THREADS; i++) {
		arr[i] = i;
		if (pthread_create(&t[i], NULL, do_compute, &arr[i])) {
			printf("error thread creation!\n");
			return 0;
		}
	}
	for (i = 0; i < MAX_THREADS; i++) {
		if (pthread_join(t[i], NULL)) {
			printf("error thread join!\n");
			return 0;
		}
	}
	for (i = 1; i < MAX_THREADS; i++) {
		assert(gvar.arr[0] == gvar.arr[i]);
	}
	return 0;
}
