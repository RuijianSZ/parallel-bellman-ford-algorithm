#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L

#define MAX_VALUE 1000000




#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int tripCount;
} pthread_barrier_t;


int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
    if(count == 0)
    {
        errno = EINVAL;
        return -1;
    }
    if(pthread_mutex_init(&barrier->mutex, 0) < 0)
    {
        return -1;
    }
    if(pthread_cond_init(&barrier->cond, 0) < 0)
    {
        pthread_mutex_destroy(&barrier->mutex);
        return -1;
    }
    barrier->tripCount = count;
    barrier->count = 0;
    
    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_mutex_lock(&barrier->mutex);
    ++(barrier->count);
    if(barrier->count >= barrier->tripCount)
    {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 1;
    }
    else
    {
        pthread_cond_wait(&barrier->cond, &(barrier->mutex));
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__















// provided via command line arguments
int NUM_THREADS;
int N_V;

int flag = 1;
int *mask, *mask1;

int **m;
int *d, *d_local;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t bar;

void swap(int *a, int *b)
{
    int temp;
    
    temp = *b;
    *b   = *a;
    *a   = temp;
}

void *ini(void *arg)
{
    int taskid;
    taskid = *(int*)arg;
    
		//printf("Thread %d\n", taskid);

    int first, last;
    
    first = (taskid * N_V)/NUM_THREADS;
    last = first + N_V/NUM_THREADS;
    
    int i;
    for (i=first; i<last; i++)
    {
        d[i] = INT_MAX;
        d_local[i] = INT_MAX;
        mask[i] = 0;
				mask1[i] = 0;
    }
    
    pthread_exit(NULL);
}

void *helper(void *arg)
{
    int taskid;
    taskid = *(int*)arg;

		//printf("Thread %d\n", taskid);
    
    int first, last;
    
    first = (taskid * N_V)/NUM_THREADS;
    last = first + N_V/NUM_THREADS;
		//printf("TID[%d]  first=%d last=%d\n", taskid, first, last);
    
    int i, j, k;
    for (k=0; k<N_V; k++)
    {
      for (i=first; i<last; i++)
      {
        for (j=0; j<N_V; j++)
        {
          if (mask[j] == 1)
          {
            if (d_local[i] == INT_MAX || d_local[i] > d[j] + m[j][i])
            {
              d_local[i] = d[j] + m[j][i];
              mask1[i] = 1;
            }
          }
        }
      }
      pthread_barrier_wait(&bar);

			for (i=first; i<last; i++) {
				if (d_local[i] != d[i]) {
					d[i] = d_local[i];
					pthread_mutex_lock(&mutex);
          flag = 1;
					pthread_mutex_unlock(&mutex);
				}
			}

			// wait for everyone to make potential updates to the global flag
      pthread_barrier_wait(&bar);

			// if global flag is 0 => no active vertices => can terminate early 
      if (flag == 0) {
				//printf("TID[%d]  k=%d\n", taskid, k);
        pthread_exit(NULL);
		  }

			// wait for everyone to check flag == 0 condition before resetting
      // it to 0 for the next iteration
      pthread_barrier_wait(&bar);

      if (taskid == 0)
      {
         flag = 0;
      }

			// update the active vertices from this iteration
			// and reset the mask1 bits for the next iteration
			// Note: not sure why swap() worked before, but it doesn't sound correct
      for (i=first; i<last; i++) {
         mask[i] = mask1[i];
				 mask1[i] = 0;
			}

			// wait for the active vertices to be updated before the next iteration
      pthread_barrier_wait(&bar);
    }
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i, j, p;

		if(argc != 3) {
			printf("Usage: %s <NumberOfVertices> <NumThreads>\n", argv[0]);
			exit(1);
		}
		N_V = atoi(argv[1]);
		NUM_THREADS = atoi(argv[2]);

		// Uncomment this to get random values, otherwise rand() will 
		// produce same values every run
		// Either way, it doesn't really matter for our purposes
		//srand(time(NULL));

    pthread_barrier_init(&bar, NULL, NUM_THREADS);
    
		d = malloc(N_V * sizeof(int));
		d_local = malloc(N_V * sizeof(int));
		mask = malloc(N_V * sizeof(int));
		mask1 = malloc(N_V * sizeof(int));
		m = malloc(N_V * sizeof(int*));
    for (p=0; p<N_V; p++)
    {
				m[p] = malloc(N_V * sizeof(int));
        for (j=0; j<N_V; j++)
        {
         		// Generate random edge weights, force more non-zeros with second rand()
            m[p][j] = rand() % MAX_VALUE + rand() % (MAX_VALUE/10 + 1);
        }
    }
   
    pthread_t thread[NUM_THREADS];
		int tid[NUM_THREADS];

    struct timespec begin, end;
		clock_gettime(CLOCK_MONOTONIC, &begin);
    for (i=0; i<NUM_THREADS; i++) 
    {
				tid[i] = i;
        pthread_create(&thread[i], NULL, ini, (void *)&tid[i]);
    }
    for (i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);
  
    d[0] = 0;
    mask[0] = 1;
    flag = 1;
    
    for (i=0; i<NUM_THREADS; i++) 
    {
        pthread_create(&thread[i], NULL, helper, (void *)&tid[i]);
		}
    
    for (i=0; i<NUM_THREADS; i++)
        pthread_join(thread[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);    
    uint64_t elapsed = BILLION * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
    printf("Elapsed time: %lf\n", (double)elapsed/BILLION);

		for(i = 0; i < N_V; i++) 
			free(m[i]);
		free(m);
		free(d);
		free(d_local);
		free(mask);
		free(mask1);
 
    return 0;
}

