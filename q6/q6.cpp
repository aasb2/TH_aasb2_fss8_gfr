#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;



typedef void*(*FUNCTION)(void *argument);

struct ARGUMENT{
	FUNCTION function;
	int argument;
	int id;
	ARGUMENT(){
		this->function = nullptr;
		this->argument = -1;
		this->id =-1;
	}
	ARGUMENT(FUNCTION function, int argument, int id){
		this->function = function;
		this->argument = argument;
		this->id =id;
	}
	ARGUMENT(FUNCTION function, int argument){
		this->function = function;
		this->argument = argument;
		this->id =-1;
	}
};




const int N = 10;
int ID = -1;

const int BUFFER_SIZE = 10;
const int RESULT_SIZE = 10;

queue<ARGUMENT> buffer;
vector<int> RESULT(RESULT_SIZE);

int BUFFER_FULL = 0;
int BUFFER_EMPTY = BUFFER_SIZE;
int RESULT_FULL = 0;
int RESULT_EMPTY = RESULT_SIZE;

//MUTEXES
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
//VARIAVEIS DE CONDIÇÃO
pthread_cond_t result_cond[RESULT_SIZE];

//BOOLEANOS
bool thread_executing;

//Inicializa as estruturas e variáveis necessárias;
void init(){
	for(int i=0; i< RESULT_SIZE ; i++)
		RESULT[i] =-1;
	
}


//FUNÇÃO QUE SERÁ EXECUTADA 
void *funexec(void *arg){
	cout<<"HERE"<<endl;
	ARGUMENT argument = *((ARGUMENT*)arg);
	int id = argument.id;
	id = 0;
	pthread_mutex_lock(&result_mutex);
		RESULT[id] = id;
	pthread_mutex_unlock(&result_mutex);
	pthread_cond_signal(&result_cond[id]);
	cout<<ID++<<endl;
	pthread_exit(NULL);
}


int main(){
	pthread_t threadId[N];
	ARGUMENT *argument = new ARGUMENT;
	for(int i=0; i<N; i++){
		pthread_create(&threadId[i],NULL, funexec,(void*)argument); //tenta criar uma nova thread
		
	}
	pthread_exit(NULL);

}
