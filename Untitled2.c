#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//symbolic constants
#define MAX_THINKING_TIME 25000
#define VERBOSE 1

//struct to contain data about the forks
typedef struct forks{
	int isUsed;
	pthread_mutex_t forkLock;
	pthread_cond_t forkCond; 
	int forkId;
}forks;

//global variables to allow functions access to
int numPhilosophers;
int numEats;
forks* forkArray;

//function to get the left fork id given a philosopher id
int getLeftForkId(int philosopherId){
	return philosopherId;
}

//function to get the right fork id given a philosopher id
int getRightForkId(int philosopherId, int numPhilosophers){
	int rightForkId = (philosopherId+1)%(numPhilosophers);
	return rightForkId;
}

//function to pickup a fork
void pickUpFork(forks* fork,int philosopherId){
	pthread_mutex_lock(&(fork->forkLock));
	//while loop to check if a fork is used or not
	while(fork->isUsed == 1){
		pthread_cond_wait(&(fork->forkCond),&(fork->forkLock));	
	}
	if(VERBOSE){
		printf("Philosopher %d is picking up fork %d! \n",philosopherId,fork->forkId);
	}
	fork->isUsed = 1;
	
}

//function to put down a fork
void putDownFork(forks* fork,int philosopherId){
	if(VERBOSE){
		printf("Philosopher %d is putting down fork %d! \n",philosopherId,(fork->forkId));
	}
	fork->isUsed = 0; 
	
	pthread_cond_signal(&(fork->forkCond));
	pthread_mutex_unlock(&(fork->forkLock));
	
}

//thread function to make a philosopher eat food
void *eatFood(void* arg){
	int index = (int)arg; 
	int numEaten = 0;
	//keep eating until eaten the specified amount
	while(numEaten < numEats){
		usleep(random() % MAX_THINKING_TIME);
		printf("Philosopher %d is thinking about how great their next meal will be! \n", index);
		//Checks whichever index of the fork is lower and chooses that one.
		if(getRightForkId(index,numPhilosophers) > getLeftForkId(index)){
			pickUpFork(&(forkArray[getLeftForkId(index)]),index);
			usleep(random() % MAX_THINKING_TIME);
			pickUpFork(&(forkArray[getRightForkId(index,numPhilosophers)]),index);
		}
		else{
			pickUpFork(&(forkArray[getRightForkId(index,numPhilosophers)]),index);
			usleep(random() % MAX_THINKING_TIME);
			pickUpFork(&(forkArray[getLeftForkId(index)]),index);
		}
		
		printf("Philosopher %d is eating like there is no tomorrow! \n", index);
		usleep(random() % MAX_THINKING_TIME);
		putDownFork(&(forkArray[getRightForkId(index,numPhilosophers)]),index);
		putDownFork(&(forkArray[getLeftForkId(index)]),index);
		usleep(random() % MAX_THINKING_TIME);
		numEaten++;
	}
	if(VERBOSE){
		printf("Philosopher %d is leaving because he is beyond full after eating %d plates of food. \n", index,numEats);
	}
	return 0;
}

//main function for the program
int main(int argc, char** argv ){
	numPhilosophers = atoi(argv[1]);
	numEats = atoi(argv[2]);
	int i,j,k;
	pthread_t* threadArray = (pthread_t*)malloc(sizeof(pthread_t)*numPhilosophers);
	forkArray = (forks*)malloc(sizeof(forks)*numPhilosophers);
	
	//create an array of forks
	for(k = 0; k < numPhilosophers; k++){
		struct forks thisFork;
		thisFork.isUsed = 0;
		pthread_mutex_init(&(thisFork.forkLock), NULL);
		pthread_cond_init(&(thisFork.forkCond), NULL);
		thisFork.forkId = k;
		forkArray[k] = thisFork;
	}
	//create an array of threads
	for(i = 0; i < numPhilosophers; i++){
		pthread_create(&threadArray[i],NULL,eatFood,(void*)i);
	}
	//join all the threads
	for(j = 0; j < numPhilosophers; j++){
		pthread_join(threadArray[j], NULL);
	}

	free(forkArray);
	free(threadArray);
	return 0;
}
