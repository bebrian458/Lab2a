#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>

// Global variables
int numthreads, numIters;
long long counter;


// Basic add routine
void add(long long *pointer, long long value){
        long long sum = *pointer + value;
        *pointer = sum;
}

// // Thread routine
// void* worker(){
//
// }

// Main routine 
int main(int argc, char *argv[]){

	// Default values
	int opt = 0, numthreads = 1, numIters = 1, counter = 0;

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

	// Create threads
	int i;
	for(i = 0; i < numthreads; i++){

	}

	// Join threads

}