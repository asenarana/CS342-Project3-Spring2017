#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <sys/wait.h>
#include <malloc.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_PHIL 27
#define MAX_TIME 60000
#define MIN_TIME 1

struct PhilInfo
{
	int index;
	int think;
	int eat;
	int countEat;
	int countPhil;
};

bool *sticks[1];
pthread_mutex_t *mutex;

void *philosopher(void *param);

int main( int argc, char *argv[])
{
	// phil <numphil> <minthink> <maxthink> <mineat> <maxeat> <dist> <count>
	int philCount = atoi(argv[1]);
	int minThink = atoi(argv[2]);
	int maxThink = atoi(argv[3]);
	int minEat = atoi(argv[4]);
	int maxEat = atoi(argv[5]);
	bool dist;
	
	int meanThink = (minThink + maxThink) / 2;
	int meanEat = (minEat + maxEat) / 2;

	pthread_t phils[philCount];
	int thinkTime;
	int eatTime;
	struct PhilInfo philInfos[philCount];
	
	if( strcmp(argv[6], "uniform") == 0)
	{
		dist = true;
	}
	else if( strcmp(argv[6], "exponential") == 0)
	{
		dist = false;
	}
	else
	{
		// error
		return 1;
	}
	mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex, NULL);
	sticks[0] = (bool *)malloc(philCount * sizeof(bool));
	for(int a = 0; a < philCount; a++)
	{
		sticks[0][a] = true;
		if(dist)	// uniform
		{
			while((eatTime = rand()) >= MAX_TIME);
			eatTime = (eatTime / 1000) + 1;
			while((thinkTime = rand()) >= MAX_TIME);
			thinkTime = (thinkTime / 1000) + 1;
		}
		else 		// exponential
		{
			while( (eatTime = -1 * log(rand()/(1.0 + RAND_MAX)) / meanEat) >= MAX_TIME);
			eatTime = (eatTime / 1000) + 1;
			while( (thinkTime = -1 * log(rand()/(1.0 + RAND_MAX)) / meanThink) >= MAX_TIME);
			thinkTime = (thinkTime / 1000) + 1;
		}
		philInfos[a].index = a;
		philInfos[a].think = thinkTime;
		philInfos[a].eat = eatTime;
		philInfos[a].countEat = atoi(argv[7]);
		philInfos[a].countPhil = philCount;
		if(pthread_create(&phils[a], NULL, &philosopher, &philInfos[a]) != 0)
		{
			// error
			return 1;
		}
	}
	for( int b = 0; b < philCount; b++)
	{
		if(pthread_join(phils[b], NULL) != 0)
		{
			// error
			return 1;
		}
	}
	free(mutex);
	free(sticks[0]);
	return 0;
}

void *philosopher(void *param)
{
	struct PhilInfo *info = (struct PhilInfo *)param;
	int eatCount = info -> countEat;
	int left;
	int right;
	int sleepTime;
	bool hungry = true;

	if( info -> index == 0)
	{
		left = info -> countPhil -1;
		right = info -> index + 1;
	}
	else if((info -> index) == (info -> countPhil -1))
	{
		left = info -> index -1;
		right = 0;
	}
	else
	{
		left = info -> index -1;
		right = info -> index + 1;
	}
	while(eatCount > 0)
	{
		// hungry: get sticks
		printf("Philosopher %d hungry\n", info -> index);
		while(hungry)
		{
			if(pthread_mutex_lock(mutex) != 0)
			{
				// error
				return 1;
			}
			if(sticks[0][left] && sticks[0][right])
			{
				sticks[0][left] = false;
				sticks[0][right] = false;
				hungry = false;
			}
			if(pthread_mutex_unlock(mutex) != 0)
			{
				// error
				return 1;
			}
		}
		// eating: sleep
		sleepTime = info -> eat;
		printf("Philosopher %d eating\n", info -> index);
		sleep(sleepTime);
		if(pthread_mutex_lock(mutex) != 0)
		{
			// error
			return 1;
		}
			sticks[0][left] = true;
			sticks[0][right] = true;
		if(pthread_mutex_unlock(mutex) != 0)
		{
			// error
			return 1;
		}
		eatCount--;
		if(eatCount == 0)
			break;
		// thinking: sleep
		sleepTime = info -> think;
		printf("Philosopher %d thinking\n", info -> index);
		sleep(sleepTime);
		hungry = true;
	}
	return 0;
}


