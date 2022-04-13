
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

//global variable declarations
int philNum;
pthread_mutex_t lock1;
sem_t *chopstick;

//function declarations
void *Diner();
void *Philosopher();
int randInt(int min, int max);

int main(int argc, char **argv)
{
	//set up our end time condition
	time_t endAt;
	time_t start = time(NULL);
	time_t end = 100;
	endAt = start + end;

	//initialize our philosopher number counter
	philNum = 0;

	//initialize our chopsticks
	chopstick = malloc(5 * sizeof(sem_t));
	int i;
	for(i = 0; i < 5; i++)
	{
		sem_init(&chopstick[i], 0, 1);
	}

	//declare the philosophers
	pthread_t philThread[5];

	//create the philosophers
	for(i = 0; i < 5; i++)
	{
		pthread_create(&philThread[i], NULL, Philosopher, NULL);
	}
	
	//loop the main thread until timeout has been reached
	while(1)
	{
		if(start < endAt)	//we have not reached the timeout
		{
			sleep(1);
			start = time(NULL);
		}
		else			//we have reached the timeout
		{
			for(i = 0; i < 5; i++)
			{
				pthread_cancel(philThread[i]);	//cancel the philosophers
			}
			break;
		}
	}
}

void *Philosopher()
{
	pthread_mutex_lock(&lock1);
	int philosopherNum = philNum;
	philNum = philNum + 1;
	pthread_mutex_unlock(&lock1);

	//continuous loop
	while(1)
	{
		if(philosopherNum % 2 == 0) //philosopher number is even	
		{
			if(philosopherNum == 0)	//special case with philosopher number 0
			{
				sem_wait(&chopstick[4]);
				sem_wait(&chopstick[philosopherNum]);
			}
			else	//general case
			{
				sem_wait(&chopstick[philosopherNum -1]);
				sem_wait(&chopstick[philosopherNum]);
			}
			printf("philosopher number %d has grabbed chopsticks and is eating.\n",philosopherNum + 1);
			//eat
			sleep(randInt(5,10));
			if(philosopherNum == 0)	//special case with philosopher number 0
			{
				sem_post(&chopstick[philosopherNum]);
				sem_post(&chopstick[4]);
			}
			else	//general case
			{
				sem_post(&chopstick[philosopherNum]);
				sem_post(&chopstick[philosopherNum -1]);
			}
			//think
			sleep(randInt(5,10));
		}
		else	//philosopher number is odd
		{
			sem_wait(&chopstick[philosopherNum]);
			sem_wait(&chopstick[philosopherNum -1]);
			printf("philosopher number %d has grabbed chopsticks and is eating.\n",philosopherNum + 1);
			//eat
			sleep(randInt(5,10));
			sem_post(&chopstick[philosopherNum -1]);
			sem_post(&chopstick[philosopherNum]);
			//think
			sleep(randInt(5,10));
		}
	}
}

//generates a random integer between min and max (inclusively)
int randInt(int min, int max)
{
	return rand() % (max + 1 - min) + min;
}
