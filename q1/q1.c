#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_THREADS 100
#define false 0
#define true 1

typedef char bool;

long int count =0;
bool isFinished =false;

void *counter(void *theadid){
	while(true){
		
		if(count < 1000000)
			count++;
		else if(count==1000000 && !isFinished){
			isFinished = true;
			printf("%ld\n",count);
			pthread_exit(NULL); 
			
		}	
		else if(isFinished)
			pthread_exit(NULL);
	}
}


int main(void){
	
	pthread_t threadId[MAX_THREADS];
	int rc, i=0;

	for(i=0; i<MAX_THREADS; i++){
		rc = pthread_create(&threadId[i],NULL, counter,NULL);
		if(rc)
			exit(-1);
		pthread_join(threadId[i],NULL);
	}
	pthread_exit(NULL);

}
