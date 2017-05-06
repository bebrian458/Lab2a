#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>	
#include <pthread.h>
#include <time.h>

// Global variables
int numthreads = 1, numIters = 1;


// Basic add routine
void add(long long *pointer, long long value){
        long long sum = *pointer + value;
        *pointer = sum;
}

// Thread routine
void* worker(void* counter){

	// Add by 1
	int i;
	for(i = 0; i < numIters; i++){

		// Without locks
		add((long long *) counter, 1);
//			fprintf(stderr, "%lld\n", *((long long*)counter));
	}

	// Add by -1
	int j;
	for(j = 0; j < numIters; j++){

		// Without locks
		add((long long *) counter, -1);
//		fprintf(stderr, "%lld\n", *((long long*)counter));

	}
	return NULL;
}	

// Main routine 
int main(int argc, char *argv[]){

	// Default values
	int opt = 0;
	long long counter = 0;

	// Timer structs
	struct timespec start, end;

	struct option longopts[] = {
		{"threads", 	required_argument, 	NULL, 't'},
		{"iterations", 	required_argument, 	NULL, 'i'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc, argv, "t:i:", longopts, NULL)) != -1){
		switch(opt){
			case 't':
				numthreads = atoi(optarg);
				break;
			case 'i':
				numIters = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Usage: ./lab1b [--threads=numthreads] [--iterations=iterations]\n");
				exit(1);
				break;
		}
	}


	// Start timer
	if(clock_gettime(CLOCK_MONOTONIC, &start) == -1){
		fprintf(stderr, "Error starting timer\n");
		exit(1);
	}

	// Allocate memory for threads
	pthread_t *threads = malloc(numthreads*sizeof(pthread_t));
	if(threads == NULL){
		fprintf(stderr, "Error allocating memory for threads\n");
		exit(1);
	}

	// Create threads
	int i;
	for(i = 0; i < numthreads; i++){
		if(pthread_create(threads+i, NULL, worker, &counter) != 0){
			fprintf(stderr, "Error creating threads\n");
			exit(1);
		}
	}

	// Join threads
	for(i = 0; i < numthreads; i++){
		if(pthread_join(*(threads+i), NULL) != 0){
			fprintf(stderr, "Error creating threads\n");
			exit(1);
		}
	}

	// Stop timer
	if(clock_gettime(CLOCK_MONOTONIC, &end) == -1){
		fprintf(stderr, "Error stopping timer\n");
		exit(1);
	}

	// Calculations
	int numops = numthreads * numIters * 2;
	long long total_time = 1000000000 * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
	long long avg_time_per_op = total_time/numops;


	// Print CSV
	fprintf(stdout, "add-none,%d,%d,%d,%lld,%lld,%lld\n",
		numthreads,numIters,numops,total_time,avg_time_per_op,counter);

	return 0;
}