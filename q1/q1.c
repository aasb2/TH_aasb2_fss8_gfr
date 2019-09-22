#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_THREADS 100 //Número de thrads que serão criadas
//"CRIA" O TIPO bool em C
#define false 0
#define true 1
typedef char bool;

long int count =0; //Contador global
bool isFinished =false; //variável que define quando a execução das threads irão terminar
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
void *counter(void *theadid){
	while(true){
		
		if(count < 1000000){ //Soma o contatodo global até 1000000
			pthread_mutex_lock(&mutex1);
			count++;
			pthread_mutex_unlock(&mutex1);
		}
		else if(count==1000000 && !isFinished){ 
			pthread_mutex_lock(&mutex1);
			isFinished = true;	
			pthread_mutex_unlock(&mutex1);				
			printf("%ld\n",count);				
		}	
		else
			break;
	}
	pthread_exit(NULL);
}


int main(void){
	
	pthread_t threadId[MAX_THREADS];
	int rc, i=0;

	for(i=0; i<MAX_THREADS; i++){
		rc = pthread_create(&threadId[i],NULL, counter,NULL);
		if(rc)
			exit(-1);
	}
	pthread_exit(NULL);

}
