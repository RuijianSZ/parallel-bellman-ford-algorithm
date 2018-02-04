#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L

#define MAX_VALUE 1000000

// provided via command line arguments
int N_V;

int **m;
int *d;


int main(int argc, char *argv[]) 
{
    int i, j, k, p;

    if(argc != 2) {
			printf("Usage: %s <NumberOfVertices>\n", argv[0]);
			exit(1);
		}
		N_V = atoi(argv[1]);

		// Uncomment this to get random values, otherwise rand() will 
		// produce same values every run
		// Either way, it doesn't really matter for our purposes
		//srand(time(NULL));

		d = malloc(N_V * sizeof(int));
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

    struct timespec begin, end;
		clock_gettime(CLOCK_MONOTONIC, &begin);
    for (i=0; i<N_V; i++)
    {
        d[i] = INT_MAX;
    }    
		
    for (k=0; k<N_V; k++)
    {
        for (i=0; i<N_V; i++)
        {
          for (j=0; j<N_V; j++)
          { 
            if (d[i] == INT_MAX || d[i] > d[j] + m[j][i])
            {
                d[i] = d[j] + m[j][i];
            }
          }
        }
    }
 
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    uint64_t elapsed = BILLION * (end.tv_sec - begin.tv_sec) + end.tv_nsec - begin.tv_nsec;
    printf("Elapsed time: %lf\n", (double)elapsed/BILLION);

		for(i = 0; i < N_V; i++) 
			free(m[i]);
		free(m);
		free(d);
     
    return 0;
}

