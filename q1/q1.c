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
		//Entra na região crítica
		pthread_mutex_lock(&mutex1); //Não permite que duas threads acessem a mesma variável e bloqueia se outra thread tentar entrar na região crítica
		if(count<1000000) // Se o contador estiver em um valor menor que 1000000 soma o 
			count++;
		//SE O CONTADOR GLOBAL FOR IGUAL A 1000000
		if(count == 1000000){
			//seta que o loop irá parar
			stop = true;
			//imprime o valor no terminal e impede que outras threads o façam
			if(!isFinished){	
				isFinished = true;	
				printf("%ld\n",count);	//Printa o valor 10000000
			}	
		}
		//sai da região crítica
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

