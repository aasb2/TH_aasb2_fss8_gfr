#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;



typedef void*(*FUNCTION)(void *argument);

struct ARGUMENT{
	FUNCTION function;
	int argument;
	int id;
	int thread_id;
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
	ARGUMENT(FUNCTION function, int argument, int id, int thread_id){
		this->thread_id = thread_id;
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




const int N = 5;
int ID = -1;

const int BUFFER_SIZE = 10;
const int RESULT_SIZE = 10;

queue<ARGUMENT> buffer;
vector<int> RESULT(RESULT_SIZE);

int BUFFER_FULL = 0;
int BUFFER_EMPTY = BUFFER_SIZE;
int RESULT_FULL = 0;
int RESULT_EMPTY = RESULT_SIZE;
int THREADS_EXECUTING = 0;


//THREADS
pthread_t threads[N];
//MUTEXES
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t available_threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t available_result_id_mutex = PTHREAD_MUTEX_INITIALIZER;
//VARIAVEIS DE CONDIÇÃO
pthread_cond_t result_cond[RESULT_SIZE];
pthread_cond_t buffer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t result_full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t result_empty_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t available_threads_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t available_result_id_cond = PTHREAD_COND_INITIALIZER;
//BOOLEANOS
bool thread_executing;
bool available_threads[N];
bool available_result_id[RESULT_SIZE];
//bool available_result[RESULT_SIZE];
//TEST
queue<int>test;
pthread_mutex_t mutest;
//Inicializa as estruturas e variáveis necessárias;
void init(){
	for(int i=0; i< RESULT_SIZE ; i++){
		RESULT[i] =-1;	
		result_cond[i] = PTHREAD_COND_INITIALIZER;
		available_result_id[i] = true;
	}
	for(int i=0; i<N; i++){
		available_threads[i] = true;
	}
}


//FUNÇÃO QUE SERÁ EXECUTADA 
void *funexec(void *arg){
	
	ARGUMENT argument = *((ARGUMENT*)arg);
	int id = argument.id;
	int thread_id = argument.thread_id;
	//cout<<"FUNEXEC RECEIVED: "<<id<<endl;
	//cout<<"THREAD ID: "<<thread_id<<endl;
	pthread_mutex_lock(&result_mutex);
		RESULT[id] = id;
	pthread_cond_signal(&result_cond[id]);
	pthread_mutex_unlock(&result_mutex);
	
	//cout<<ID++<<endl;
	pthread_mutex_lock(&available_threads_mutex);
		available_threads[thread_id] = true;
		THREADS_EXECUTING --;
		//cout<<endl;
		//cout<<"FUNEXEC FINISHED: "<<id<<endl;
		pthread_cond_signal(&available_threads_cond);
	pthread_mutex_unlock(&available_threads_mutex);
	//cout<<argument.argument<<endl;
}

int agendarExecucao(ARGUMENT argument){
	bool isFree = false;
	int id = -1;
	pthread_mutex_lock(&available_result_id_mutex);
		while(RESULT_FULL == RESULT_SIZE)
			pthread_cond_wait(&available_result_id_cond,&available_result_id_mutex);
		for(int i=0; i<RESULT_SIZE && !isFree; i++){
			if(available_result_id[i]){
				isFree = true;
				id = i;
				available_result_id[i] = false;
			}
		}
		RESULT_FULL++;
		RESULT_EMPTY--;
	pthread_mutex_unlock(&available_result_id_mutex);
	pthread_mutex_lock(&buffer_mutex);
		while(BUFFER_FULL == BUFFER_SIZE)
			pthread_cond_wait(&buffer_full_cond,&buffer_mutex);
		argument.id = id;
		buffer.push(argument);
		BUFFER_FULL++;
		BUFFER_EMPTY--;
		pthread_cond_signal(&buffer_empty_cond);
	pthread_mutex_unlock(&buffer_mutex);
	//ARGUMENT *arg =  new ARGUMENT;
	//*arg = buffer.front();
	//arg->function((void*)arg);
	//RESULT[id] = id;
	return id;
}

int pegarResultadoExecucao(int id){
	int result;
	pthread_mutex_lock(&result_mutex);
		while(RESULT[id] == -1)
			pthread_cond_wait(&result_cond[id], &result_mutex);
		result = RESULT[id];
		RESULT[id] = -1;
		pthread_mutex_lock(&available_result_id_mutex);
			available_result_id[id] = true;
			RESULT_FULL --;
			RESULT_EMPTY ++;
			pthread_cond_signal(&available_result_id_cond);
		pthread_mutex_unlock(&available_result_id_mutex);
	pthread_mutex_unlock(&result_mutex);
	
	return result;

}

void *despachante(void *ignore){
	int bug = 0;
	while(true){
		cout<<"desp"<<endl;
		ARGUMENT *argument = new ARGUMENT;
		
		int threadId = -1;
		
		pthread_mutex_lock(&buffer_mutex);
			while(BUFFER_EMPTY == BUFFER_SIZE)
				pthread_cond_wait(&buffer_empty_cond, &buffer_mutex);
			*argument = buffer.front();
			buffer.pop();
			BUFFER_FULL--;
			BUFFER_EMPTY++;
			pthread_cond_signal(&buffer_full_cond);
		pthread_mutex_unlock(&buffer_mutex);
		//cout<<"THE BUG IS"<<bug++<<endl;
		pthread_mutex_lock(&available_threads_mutex);
			bool isFree = false;
			while(THREADS_EXECUTING == N)
				pthread_cond_wait(&available_threads_cond, &available_threads_mutex);
			for(int i=0; i<N && !isFree; i++){
				if(available_threads[i]){
					threadId = i;
					pthread_cancel(threads[i]);
					available_threads[i] = false;
					(*argument).thread_id = i;
					isFree = true;
				}
			
			}
			
			THREADS_EXECUTING++;
		pthread_mutex_unlock(&available_threads_mutex);
		//cout<<"THE BUG IS: "<<bug++<<endl;
		//cout<<"THREAD ID DESPACHANTE = "<<(*argument).thread_id<<endl;
		pthread_create(&threads[threadId],NULL,(*argument).function,(void *)argument);
	}
}

void *prod(void * arg){
	while(true){
		cout<<"prod"<<endl;
		pthread_mutex_lock(&mutest);
		test.push(agendarExecucao(ARGUMENT(funexec,10)));
		pthread_mutex_unlock(&mutest);
		
	}
	pthread_exit(NULL);
}
void *cons(void * arg){
	while(true){
		cout<<"cons"<<endl;
		pthread_mutex_lock(&mutest);
		if(!test.empty()){
			cout<<pegarResultadoExecucao(test.front())<<endl;
			test.pop();
		}
		pthread_mutex_unlock(&mutest);
	}
	pthread_exit(NULL);
}


int main(){
	init();
	for(int i=0; i<N; i++)
		cout<<available_threads[i]<<" ";
	cout<<endl;
	pthread_t despachante_thread;
	pthread_t prodt, conmt;
	ARGUMENT *argument = new ARGUMENT;
	pthread_create(&despachante_thread,NULL,despachante,NULL);
	//pthread_create(&prodt,NULL,prod,NULL);
	//pthread_create(&conmt,NULL,cons,NULL);
	for(int i =0; i<200; i++){
		cout<<agendarExecucao(ARGUMENT(funexec,10))<<endl;
		cout<<"A"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<pegarResultadoExecucao(i)<<endl;
		cout<<"B"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<agendarExecucao(ARGUMENT(funexec,10))<<endl;
		cout<<"C"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<pegarResultadoExecucao(i)<<endl;
		cout<<"D"<<endl;
	}
	pthread_exit(NULL);

}

