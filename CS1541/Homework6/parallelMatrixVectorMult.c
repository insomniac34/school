//Written by Tyler Raborn

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

int N;
int P;
int rowsPerStruct;

float** a ;
float* b ;
float* x ;

//Tyler's variables:
typedef struct  //a struct for holding the individual data to be calculated
{
	//the following need malloc'd or assigned to hold vals:
	float *row; 
	float *vec;

	float **multiRow;

	int ID;
	int *IDlist;
} dotProduct;

dotProduct *dotList;
pthread_t *threadList; //array of threads
float *result;

static char *THREAD_CREATE_ERROR = "Unable to create thread!\n";
static char *THREAD_JOIN_ERROR = "Unable to join thread(s)!\n";

void *parallelMult();
void *parallelMultB();
static void error(const char*);
void cleanup();

int main (int argc, char *argv[] )    
{
	/* the array size should be supplied as a command line argument */
	if(argc != 3) 
	{
		printf("wrong number of arguments. Usage: ./program <N> <P>");
		exit(2);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
	if (P > N) P = N;
	rowsPerStruct = (N / P);

	printf("Array size = %d, Number of Threads: %d \n", N, P );
	int mid = (N+1)/2;
	int i, j;
	double time_start, time_end;
	struct timeval tv;
	struct timezone tz;

	/* allocate arrays dynamically */
	a = malloc(sizeof(float*)*N);

	for (i = 0; i < N; i++) 
	{
		a[i] = malloc(sizeof(float)*N);
	}

	b = malloc(sizeof(float)*N);
	x = malloc(sizeof(float)*N);

	/* Inititialize matrix A and vector B. */
	for (i=0; i<N; i++) 
	{
		for (j=0; j<N; j++) 
		{
			if (j == i)                    
			{ 
				a[i][j] = 2.0; 
			}
			else if (j == i-1 || j == i+1) 
			{ 
				a[i][j] = 1.0; 
			}
			else                           
			{ 
				a[i][j] = 0.01;
			}
		}
		b[i] = mid - abs(i-mid+1);
	}

	if (P==N)
	{
		//printf("P = N or P==1");
		//allocate work to be done:
		if (P>1)
		{
			dotList = (dotProduct*)malloc(sizeof(dotProduct)*P);
			result = (float*)malloc(sizeof(float)*N);
		}

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		for (i=0; i<N; i++) 
		{
			x[i] = 0.0;
		}
		for (i=0; i<N; i++) 
		{
			if (P>1)
			{
				dotList[i].row = (float*)malloc(sizeof(float)*N); //allocate space to hold the dot product of each matrix.
				dotList[i].vec = b; //set pointer equal to the vector.
				dotList[i].ID = i;
			}
			for (j=0; j<N; j++) 
			{
				if (P>1) 
				{
					dotList[i].row[j] = a[i][j];
				}
				else x[i] += a[i][j] * b[j];
				//else dotList[i].row[j] = a[i][j]; //store the current row of the matrix
			}
		}

		//now we have an array of structs holding all of the data to be calculated. it's time to parallelize.
		//set up threads:
		if (P > 1)
		{
			threadList = (pthread_t*)malloc(sizeof(pthread_t)*P);
			int newThread = 0;
			for (i = 0; i < P; i++) //for each thread P, spawn a copy to compute the matrix sum
			{
				dotProduct *dp = &dotList[i];
				newThread = pthread_create(&threadList[i], NULL, &parallelMult, (void*)dp);
				if (newThread!=0) error(THREAD_CREATE_ERROR);
			}

			for (i = 0; i < P; i++)
			{
				pthread_join(threadList[i], NULL);
			}
		}

		gettimeofday (&tv ,  &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		if (P==1)
		{
			/* this is for checking the results */
			printf("\nSingle-Threaded Results for (%dx%d) Matrix-Vector Multiplication:\n", N, N);

			for (i=0; i<N; i++) 
			{
				printf("%f\n",x[i]); 
			}
			printf ("time = %lfns\n", time_end - time_start);
		}
		else
		{
			printf("\n%d-Threaded Results for (%dx%d) Matrix-Vector Multiplication:\n", P, N, N);

			int k;
			for (k = 0; k < N; k++)
			{
				printf("%f\n", result[k]);
			}

			printf ("time = %lfns\n", time_end - time_start);
		}
		atexit(cleanup);
		exit(0);
	}
	else if (N > P)
	{
		printf("%d > %d. dividing up the work.\n", N, P);
		result = (float*)malloc(sizeof(float)*N);    
		dotList = (dotProduct*)malloc(sizeof(dotProduct)*P);

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		for (i = 0; i < P; i++)
		{
			dotList[i].multiRow = (float**)malloc(sizeof(float*)*rowsPerStruct);
			dotList[i].IDlist = (int*)malloc(sizeof(int)*rowsPerStruct);
			dotList[i].vec = b;
		}

		gettimeofday (&tv ,   &tz);
		time_start = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		int q;
		int resultIndex = 0;
		int index = 0;
		for (i=0; i<P; i++) 
		{
			
			for (j=0; j<rowsPerStruct; j++) 
			{
				dotList[i].IDlist[j] = resultIndex;
				dotList[i].multiRow[j] = (float*)malloc(sizeof(float)*N);
				resultIndex++;
				for (q=0;q<N;q++)
				{
					dotList[i].multiRow[j][q] = a[index][q]; 
				}
				index++;
			}

		}

		threadList = (pthread_t*)malloc(sizeof(pthread_t)*P);

		int newThread = 0;
		for (i = 0; i < P; i++) //for each thread P, spawn a copy to compute the matrix sum
		{
			dotProduct *dp = &dotList[i];
			newThread = pthread_create(&threadList[i], NULL, &parallelMultB, (void*)dp);
			if (newThread!=0) 
			{
				error(THREAD_CREATE_ERROR);
			}
		}

		int threadJoin = 0;
		for (i = 0; i < P; i++) //join the threads
		{
			//void *status;
			threadJoin = pthread_join(threadList[i], NULL);
			if (threadJoin!=0) 
			{
				error(THREAD_JOIN_ERROR);
			}
		}

		gettimeofday (&tv ,  &tz);
		time_end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;

		printf("\n%d-Threaded Results for (%dx%d) Matrix-Vector Multiplication:\n", P, N, N);
		//printf("result = {");
		int k;
		for (k = 0; k < N; k++)
		{
			printf("%f\n", result[k]);
		}
		printf ("time = %lfns\n", time_end - time_start);
		//printf("}\n");

		for (i = 0; i < P; i++)
		{
			for (j = 0; j < rowsPerStruct; j++)
			{
				free(dotList[i].multiRow[j]); 
			}

			free(dotList[i].multiRow);
			free(dotList[i].IDlist);
		}

		atexit(cleanup);
		exit(0);
	}

	return 0;
}

void *parallelMult(void *args) 
{
	dotProduct *localData = (dotProduct*)args;
	result[localData->ID] = 0.0;
	int i;
	for (i = 0; i < N; i++)
	{
		printf("writing (%g*%g) to struct ID # %d\n", localData->row[i], localData->vec[i], localData->ID);
		result[localData->ID] += (localData->row[i] * localData->vec[i]);
	}

}

void *parallelMultB(void *args)
{
	dotProduct *localData = (dotProduct*)args;
	int i, j;
	for (i = 0; i < rowsPerStruct; i++)
	{
		result[localData->IDlist[i]]=0.0; 
	}

	for (i = 0; i < rowsPerStruct; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("writing (%g*%g) to struct ID # %d\n", localData->multiRow[i][j], localData->vec[j], localData->IDlist[i]);
			result[localData->IDlist[i]] += (localData->multiRow[i][j] * localData->vec[j]);
		}
	}  
}

static void error(const char *msg) //for error handling
{
	perror(msg);
	exit(-1);
}

void cleanup()
{
	int i;
	for (i = 0; i < N; i++)
	{
		free(a[i]);  
	}

	free(a);
	free(b);
	free(x);
	free(threadList);
	free(dotList);
	free(result);
}

