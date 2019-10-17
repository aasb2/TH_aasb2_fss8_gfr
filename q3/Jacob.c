#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define ITERACOES 1000
//DIVISAO INCOGNITAS/THREADS
//#define THREADS 1
int THREADS;
int INCOGNITAS;
int DIVISAO;

//PREENCHER Matriz  Exemplo, 
int A[4][4] =  {{1,0, 0, 3},
		{0,1, 1,0},
		{1,0, 3,0},
		{0,0, 1,1}}; 
int B[4] = {11,
	    13, 
	    5, 
	    7};
float X[4][2] = {{1,1},
		 {1,1},
		 {1,1},
		 {1,1}}; //Solucoes Xi^(0) duplicadas,(Lado Esquerdo e lado direito irão intercalar durante a execução);

/*int A[2][2] =  {{2,1},
		{5,7}}; 
int B[2] = {11,
	    13};
float X[2][2] = {{1,1},
		 {1,1}};*/
int **TC;
int *Mais;

pthread_barrier_t barrier;

void *iteracao();

int main() {

	int i, T, I; int threadTal, cont = 0, Coluna = 0;
	scanf("%d %d", &T, &I);   //Quantidade de Threads e Incognitas
	THREADS = T;
	INCOGNITAS = I;
	DIVISAO = I/T;	

	TC = (int**) malloc (sizeof(int*)*T);
	Mais = (int*) malloc (sizeof(int)*T);
	for(int z = 0; z < THREADS; z++){
		TC[z] = (int*) malloc (sizeof(int)*(DIVISAO+1));	//Mais uma, caso Alguma thread fique com mais incognitas que outra;
	} 
	
	for(int q = 0; q < INCOGNITAS; q++){
		if(cont == THREADS){
		Coluna++;
		cont = 0;		
		}
		threadTal = q%THREADS;
		TC[threadTal][Coluna] = q;
		if(q >= (INCOGNITAS-(INCOGNITAS%THREADS))){
			Mais[threadTal] = 1;		
		}	
		else{
			Mais[threadTal] = 0;
		}
		cont++;
	}

	pthread_t threads[THREADS]; 
  	int *ids[THREADS]; 
	pthread_barrier_init(&barrier, NULL, THREADS);	
	
	for(i = 0; i < THREADS; i++) {
		ids[i] =(int *) malloc(sizeof(int)); 
		*ids[i] = i;
  	pthread_create(&threads[i],NULL,iteracao,(void *) ids[i]);  
	}
  
  for(i = 0; i < THREADS; i++) { pthread_join(threads[i],NULL); }
  
  pthread_barrier_destroy(&barrier);
	for(int y = 0; y < THREADS; y++){
		free(TC[y]);
	}
   free(Mais);
	free(TC);
  pthread_exit(NULL);
  
  return 0;
  
}

void *iteracao(void *threadid){

	int idthread = *((int*) threadid);
	int MaisI = Mais[idthread];
	float Form, ab, somatorio, xjk;
	float aii, bi, aij;
	int impar, par, id;
	for (int i=0; i < ITERACOES; i++) {
	
	for(int k = 0; k < DIVISAO+MaisI; k++){
	id = TC[idthread][k];  //Incognita que a thread é responsável.
	ab = 0;
	aii = A[id][id];
	bi = B[id];
	impar = i%2;
	par = (impar+1)%2;
	for(int j = 0; j < INCOGNITAS; j++){
		aij = A[id][j];
		xjk = X[j][impar];  //Seleciona Lado ESquerdo ou direito da Solucao;
		if(j != id){
		ab +=  aij*xjk;
		}
	}
	somatorio = ab;	
	Form = (1/aii)*(bi-somatorio);
	X[id][par] = Form;
	if(i == ITERACOES-1){
	printf("Ultima Solucao Incognita %d = %f \n", id, Form);
	}
	}
	pthread_barrier_wait(&barrier);
  }

	pthread_exit(NULL);
}
