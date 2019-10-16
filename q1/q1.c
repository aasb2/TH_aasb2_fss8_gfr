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
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
void *counter(void *theadid){
	bool stop = false;	
	while(!stop){ //Soma o contatodo global até 1000000
		pthread_mutex_lock(&mutex1); //Não permite que duas threads acessem a mesma variável e bloqueia
		if(count<1000000)
			count++;
		else if(count == 1000000){
			stop = true;
			if(!isFinished){	
				isFinished = true;	
				printf("%ld\n",count);	//Printa o valor 10000000
			}	
		}
		pthread_mutex_unlock(&mutex1); //termina bloqueio
		
	}
	//pthread_mutex_lock(&mutex2); 
	//Impede que outra thread acesse primeiro a variável isFinished
		
	//pthread_mutex_unlock(&mutex2);	
	//Desbloqueia a região

	pthread_exit(NULL);
}


int main(void){
	
	pthread_t threadId[MAX_THREADS]; //Cria um array de threads
	int rc, i=0;

	for(i=0; i<MAX_THREADS; i++){
		rc = pthread_create(&threadId[i],NULL, counter,NULL); //tenta criar uma nova thread
		if(rc) //em caso de erro encerra a execução
			exit(-1);
	}
	pthread_exit(NULL);

}
