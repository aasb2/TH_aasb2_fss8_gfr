#include <bits/stdc++.h>

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
const int MAX_SIZE = 1e5;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t resultCond[MAX_SIZE];
pthread_cond_t buffer_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;
pthread_t thread[N];
int RESULT[MAX_SIZE];
queue<ARGUMENT> BUFFER;
int FULL = 0;
int EMPTY = MAX_SIZE;


void init(){
	for(int i =0; i<MAX_SIZE; i++)
		resultCond[i] = PTHREAD_COND_INITIALIZER;
	fill_n(RESULT,MAX_SIZE,-1);
}

void *funexec(void *argument){
	pthread_mutex_lock(&buffer_mutex);
		ARGUMENT *arg = (ARGUMENT*)argument;
		int	id = arg->id;
		RESULT[id] = id;
		pthread_cond_signal(&resultCond[id]);
	pthread_mutex_unlock(&buffer_mutex);
}

int agendarExecucao(ARGUMENT argument){
	int id = ID++;
	argument.id = id;
	pthread_mutex_lock(&buffer_mutex);
		while(FULL == MAX_SIZE)
			pthread_cond_wait(&buffer_full,&buffer_mutex);
		BUFFER.push(argument);
		EMPTY--;
		FULL++;
	pthread_mutex_unlock(&buffer_mutex);
	return id;
}

int pegarResultadoExecucao(int id){
	
	pthread_mutex_lock(&result_mutex);
		while(RESULT[id] == -1)
			pthread_cond_wait(&resultCond[id], &result_mutex);
	pthread_mutex_unlock(&result_mutex);
}

void *despachante(void *argument){
	ARGUMENT arg;
	pthread_mutex_lock(&buffer_mutex);
		while(BUFFER.empty())
			pthread_cond_wait(&buffer_empty,&buffer_mutex);
		arg = BUFFER.front();
		BUFFER.pop();
		EMPTY++;
		FULL--;
	pthread_mutex_unlock(&buffer_mutex);


	pthread_exit(NULL);	
}

int main(){


}
