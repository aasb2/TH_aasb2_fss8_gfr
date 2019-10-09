#include <bits/stdc++.h>

using namespace std;

const int N = 10;
int ID = -1;
int RES =0;
const int MAX_SIZE = 1e5;
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t isReady = PTHREAD_COND_INITIALIZER;
typedef void*(*FUNCTION)(void *argument);
template <class T>
struct ARGUMENT{
	FUNCTION function;
	T argument;
	int id;
	ARGUMENT(){
		this->function = nullptr;
		this->argument = NULL;
		this->id =-1;
	}
	ARGUMENT(FUNCTION function, T argument){
		this->function = function;
		this->argument = argument;
		this->id = id;
	}
	ARGUMENT(FUNCTION function, T argument){
		this->function = function;
		this->argument = argument;
		this->id = -1;
	}
};


queue<ARGUMENT> BUFFER;
int RESULT[MAX_SIZE];

void *funexec(void *argument){
	return RES++;
}

int agendarExecucao(ARGUMENT argument){
	pthred_mutex_lock(&buffer_mutex);
	BUFFER.push_back(argument);
	pthread_mutex_unlock(&buffer_mutex);
	return ++ID;
}

int pegarResultadoExecucao(int id){
	
	pthred_mutex_lock(&buffer_result);
		while(buffer_result[id] == -1)
			pthread_cond_wait(&isReady, &buffer_result);
	pthread_mutex_unlock(&buffer_result);
}


int main(){


}
