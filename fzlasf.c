#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

#define THINKMIN 1
#define THINKMAX 20
#define EATMIN 2
#define EATMAX 9

pthread_t philosopher[5];
pthread_mutex_t eating_fork[5];

int forks[5] = {0,0,0,0,0};

int get_rand(int min, int max){
	return genrand_int32 () % (max-min) + min;
}

/* 
 * Prints the available forks. 0 for unused, and the philosopher's number for the used.
 */
void print_forks(int tab){
	int i = 0;
	if(tab)
		printf("\t\t");
	printf("Forks in use: {");
	for(i=0; i<5; ++i){
		printf(" %d ", forks[i]);
	}
	printf("}\n");
}

/*
 * Each philosopher is in an endless loop of eating and thinking.
 * They need two forks, which are always the same forks. 
 * Because we grab the forks simultaneously, they cannot deadlock.
 * @param n: philosopher number
 */
void *phil(int n){
	while(1){
		// Think
		int thinking_time = get_rand(THINKMIN,THINKMAX);
		printf("Philosopher %d is thinking for %d seconds.\n", n+1, thinking_time);
		sleep(thinking_time);

		// Get forks
		/*
		 * We get the forks at the phil's spot, then the +1 spot for everybody but the 5th philosopher.
		 * This ensures no deadlock.
		 * For instance, if philosopher 1,2,3,4 all grabbed their first fork at the exact same time, and were waiting for the second one, then 5 is already blocked to try and grab its n+1, or 1st fork. In this case, 4 can eat, and then unblock 3, then 2, etc. 
		 * If before this, 5 grabbed its fork first (fork #1), that blocks philosopher 1 and 5 can eat, since it can get fork #5. If 4 had grabbed fork #4, then that means philosopher 4 is already eating since it can only grab fork 5 if it has fork 4.
		 * In this way, every case possible, everybody cannot be blocked. If 4 are, then 1 is guaranteed to be eating already.
		 * If the sleep() calls are changed to usleep to test, then you will find that it never stops, and therefore never deadlocks.
		 */
		if(n!=4){
			pthread_mutex_lock(&eating_fork[n]);
			pthread_mutex_lock(&eating_fork[(n+1)%5]);
		} else {
			pthread_mutex_lock(&eating_fork[(n+1)%5]);
			pthread_mutex_lock(&eating_fork[n]);
		}
		forks[n] = n+1;
		forks[(n+1)%5] = n+1;

		// Eat
		int eating_time = get_rand(EATMIN,EATMAX);
		printf("\t\tPhilosopher %d is eating for %d seconds.\n", n+1, eating_time);
		print_forks(1);
		sleep(eating_time);
		
		// Return Forks
		forks[n] = 0;
		forks[(n+1)%5] = 0;
		pthread_mutex_unlock(&eating_fork[n]);
		pthread_mutex_unlock(&eating_fork[(n+1)%5]);
	}
}

int main(){
	int i;
	for(i=0; i<5; ++i){
		pthread_mutex_init(&eating_fork[i], NULL);
	}
	for(i=0; i<5; ++i){
		pthread_create(&philosopher[i], NULL, (void *) phil, (void*)i);
	}

	//this should never happen...
	for(i=0; i<5; ++i){
		pthread_join(philosopher[i], NULL);
	}
	for(i=0; i<5; ++i){
		pthread_mutex_destroy(&eating_fork[i]);
	}

	return 0;
}
