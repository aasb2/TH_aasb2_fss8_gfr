#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define ITERACOES 1000
//DIVISAO INCOGNITAS/THREADS
int THREADS;
int INCOGNITAS; //A quantidade de incognitas deve ser igual ao valor correspondente na matriz exemplo;
int DIVISAO;

//PREENCHER Matriz  Exemplo, --- Quatro incognitas, 4 equações
int A[2][3] =  {{2,1,11},
		{5,7,13}}; 
int B[2] = {11,
	    13};
float X[4][2] = {{1,1},
		 {1,1},
		 {1,1},
		 {1,1}}; //Solucoes Xi^(0) duplicadas,(Lado Esquerdo e lado direito irão intercalar durante a execução), Uma vai ser Xi^(k), a outra Xi^(k+1), alternando a cada iteraçao;

/*int A[2][2] =  {{2,1},
		{5,7}}; 
int B[2] = {11,
	    13};
float X[2][2] = {{1,1},
		 {1,1}};*/
int **TC;  //TC, guarda as Incognitas que a thread vai ser responsável, se quantidade de THREAADS = INCOGNITAS TC vai ser uma unica incognita para cada thread;
int *Mais;  //Caso uma thread i cuide de mais uma incognita do que outra, Mais[i] = 1, se não, significa que todas as threads cuidam de quantidades iguais de incognitas, então 0;

pthread_barrier_t barrier;

void *iteracao();

int main() {

	int i, T, I; int threadTal, cont = 0, Coluna = 0;
	scanf("%d %d", &T, &I);   //Quantidade de Threads e Incognitas
	if (T > I)
		T = I;
	THREADS = T;
	INCOGNITAS = I;
	DIVISAO = I/T;	

	TC = (int**) malloc (sizeof(int*)*T);
	Mais = (int*) malloc (sizeof(int)*T);
	for(int z = 0; z < THREADS; z++){
		TC[z] = (int*) malloc (sizeof(int)*(DIVISAO+1));	//Mais uma, caso Alguma thread fique com mais incognitas que outra;
	} 
	
	//Divide as incognitas igualmente entre as threads, caso não seja possível, adiciona mais uma para a primeira thread cuidar, e para a proxima..., de modo que nenhuma thread vai ficar com duas incognitas a mais que outra.
	for(int q = 0; q < INCOGNITAS; q++){
		if(cont == THREADS){
			Coluna++;
			cont = 0;
		}
		threadTal = q%THREADS;
		TC[threadTal][Coluna] = q;
		if(q >= (INCOGNITAS-(INCOGNITAS%THREADS))){ //Maior do que um numero divisivel igualmente entre as threads;
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
			for(int j = 0; j < INCOGNITAS; j++){ //Somatorio para AijXj^(k)
				aij = A[id][j];
				xjk = X[j][impar];  //Xj^(k);
				if(j != id){
					ab +=  aij*xjk; // Aij (tal que) j != i 
				}
			}
			somatorio = ab;	
			Form = (1/aii)*(bi-somatorio); //Fórmula Jacobiano
			X[id][par] = Form;  //Xi^(k+1)
			if(i == ITERACOES-1){
				printf("Ultima Solucao Incognita %d = %f \n", id, Form);
			}
		}
		pthread_barrier_wait(&barrier);
  	}

	pthread_exit(NULL);
}
