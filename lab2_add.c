#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>	
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <ctype.h> // isdigit()

// Global variables
int numthreads = 1, numIters = 1, opt_yield = 0, spin_lock = 0;
char m_sync = 0;
pthread_mutex_t mutex;

// Basic add routine
void add(long long *pointer, long long value){
        long long sum = *pointer + value;
        if(opt_yield)
        	sched_yield();
        *pointer = sum;
}

// Thread routine
void* worker(void* counter){

	// Add by 1
	int i;
	for(i = 0; i < numIters; i++){

		// Mutex
		if(m_sync == 'm'){
			pthread_mutex_lock(&mutex);
			add((long long *)counter, 1);
			pthread_mutex_unlock(&mutex);
		}

		// Spin-lock
		else if(m_sync == 's'){
			while(__sync_lock_test_and_set(&spin_lock, 1))
				;
			add((long long *) counter, 1);
			__sync_lock_release(&spin_lock);
		}

		// Compare-and-swap
		else if(m_sync == 'c'){
			long long old, new;
			do{
				old = *(long long *)counter;
				new = old + 1;
				if(opt_yield)
					sched_yield();
			}while(__sync_val_compare_and_swap((long long*)counter, old, new) != old);
		}

		// Without locks
		else
			add((long long *) counter, 1);
//		fprintf(stderr, "%lld\n", *((long long*)counter));
	}

	// Add by -1
	int j;
	for(j = 0; j < numIters; j++){

		// Mutex
		if(m_sync == 'm'){
			pthread_mutex_lock(&mutex);
			add((long long *)counter, -1);
			pthread_mutex_unlock(&mutex);
		}

		// Spin-lock
		else if(m_sync == 's'){
			while(__sync_lock_test_and_set(&spin_lock, 1))
				;
			add((long long *) counter, -1);
			__sync_lock_release(&spin_lock);
		}

		// Compare-and-swap
		else if(m_sync == 'c'){
			long long old, new;
			do{
				old = *(long long *)counter;
				new = old - 1;
			}while(__sync_val_compare_and_swap((long long *)counter, old, new) != old);
		}

		// Without locks
		else
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
		{"yield", 		no_argument, 		NULL, 'y'},
		{"sync", 		required_argument, 	NULL, 's'},
		{0,0,0,0}
	};

	while((opt = getopt_long(argc, argv, "t:i:ys:", longopts, NULL)) != -1){
		switch(opt){
			case 't':
				numthreads = atoi(optarg);
				if(!isdigit(*optarg)){
					fprintf(stderr, "Incorrect argument for threads. Input an integer or use default of value 1\n");
					exit(2);
				}
				break;
			case 'i':
				numIters = atoi(optarg);
				if(!isdigit(*optarg)){
					fprintf(stderr, "Incorrect argument for iterations. Input an integer or use default of value 1\n");
					exit(2);
				}
				break;
			case 'y':
				opt_yield = 1;
				break;
			case 's':
				m_sync = *optarg;
				if(m_sync != 'm' && m_sync != 's' && m_sync != 'c'){
					fprintf(stderr, "Incorrect argument for sync. Use m for mutex, s for spin-lock, or c for compare-and-swap\n");
					exit(2);
				}
				break;
			default:
				fprintf(stderr, "Usage: ./lab1b [--threads=numthreads] [--iterations=iterations] [--yield] [--sync=[m, s, or c]]\n");
				exit(1);
				break;
		}
	}

	// Initialize mutex
	if(m_sync == 'm')
		pthread_mutex_init(&mutex, NULL);

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

	// Create name
	char message[15] = "add";
	if(opt_yield){
		const char name[7] = "-yield";
		strcat(message, name);
	}
	if(m_sync == 'm'){
		const char name[3] = "-m";
		strcat(message, name);
	}
	else if(m_sync == 's'){
		const char name[3] = "-s";
		strcat(message, name);
	}
	else if(m_sync == 'c'){
		const char name[3] = "-c";
		strcat(message, name);
	}
	else{
		const char name[6] = "-none";
		strcat(message, name);
	}

	// Print CSV
	fprintf(stdout, "%s,%d,%d,%d,%lld,%lld,%lld\n",
		message,numthreads,numIters,numops,total_time,avg_time_per_op,counter);

	return 0;
}