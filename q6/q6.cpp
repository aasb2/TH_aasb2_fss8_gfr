#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;


//PARA SER MAIS FÁCIL A LEITURA , DEFINE FUNCTION COMO UMA FUNÇÃO void *(void*)
typedef void*(*FUNCTION)(void *argument);

//STRUCT QUE TERÁ O ARGUMENTO QUE SERÁ RECEBIDO PELAS THREADS
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
bool available_threads[N]; //RESPONSÁVEL POR VER QUAIS THREADS ESTÃO DISPONÍVEIS
bool available_result_id[RESULT_SIZE]; //RESPONSÁVEL POR VER QUAIS IDS ESTÃO DISPONÍVEIS


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
	//DESENCAPSULA O ARGUMENTO RECEBIDO
	ARGUMENT argument = *((ARGUMENT*)arg);
	//PEGA O ID DA REQUISIÇÃO
	int id = argument.id;
	//PEGA O ID DA THREAD
	int thread_id = argument.thread_id;
	
	//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEL (VETOR "RESULT")
	pthread_mutex_lock(&result_mutex);
		//EXECUTA A TAREFA E COLOCA O RESULTADO NO VETOR "RESULT"
		RESULT[id] = id;
	//CASO O USUÁRIO ESTEJA ESPERANDO PELO RESULTADO ACORDA ELE
	pthread_cond_signal(&result_cond[id]);
	//SAI DA REGIÃO CRÍTICA
	pthread_mutex_unlock(&result_mutex);
	
	//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEL (VETOR "available_threads")
	pthread_mutex_lock(&available_threads_mutex);
		//SINALIZA QUE ESTA THREAD ESTÁ DISPONÍVEL E TERMINOU A EXECUÇÃO
		available_threads[thread_id] = true;
		//SINALIZA QUE TEM MENOS UMA THREAD EXECUTANDO
		THREADS_EXECUTING --;
	//ENVIA UM SINAL PARA ACORDAR A THREAD DESPACHANTE CASO ELA ESTEVA ESPERANDO QUE ALGUMA THREAD FIQUE DISPONÍVEL
	pthread_cond_signal(&available_threads_cond);
	//SAI DA REGIÃO CRÍTICA	
	pthread_mutex_unlock(&available_threads_mutex);
	
	
	
}

//AGENDA A EXECUÇÃO DE UMA FUNÇÃO
int agendarExecucao(ARGUMENT argument){
	bool isFree = false;
	int id = -1;
	//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEL ("VETOR "available_result_id)
	pthread_mutex_lock(&available_result_id_mutex);
		//ENQUANTO NÃO HÁ ID DISPONÍVEIS FAZ O USUÁRIO DORMIR
		while(RESULT_FULL == RESULT_SIZE)
			pthread_cond_wait(&available_result_id_cond,&available_result_id_mutex);
		//QUANDO HÁ ID DISPONÍVEIS, PEGA O PRIMEIRO ID DISPONÍVEL
		//SETA ELE DE VOLTA COMO INDISPONÍVEL E ENCERRA O LOOP
		for(int i=0; i<RESULT_SIZE && !isFree; i++){
			if(available_result_id[i]){
				isFree = true;
				id = i;
				available_result_id[i] = false;
			}
		}
		//SINALIZA QUE TEM  MAIS UM ID INDISPONÍVEL
		RESULT_FULL++;
		RESULT_EMPTY--;
		//SAI DA REGIÃO CRÍTICA
	pthread_mutex_unlock(&available_result_id_mutex);
	//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEÇ (VARIÁVEL "BUFFER")
	pthread_mutex_lock(&buffer_mutex);
		//ENQUANTO O BUFFER ESTIVER CHEIO BLOQUEIA O USUÁRIO ATÉ QUE HAJA UM ESPAÇO DISPONÍVEL
		while(BUFFER_FULL == BUFFER_SIZE)
			pthread_cond_wait(&buffer_full_cond,&buffer_mutex);
		//SETA O ID NA VARIÁVEL "argument" PARA QUE A FUNÇÃO SAIBA EM QUE POSIÇAO DO BUFFER DE RESULTADOS O RESULTADO DA EXECUÇÃO DA FUNÇÃO SERÁ COLOCADO
		argument.id = id;
		//INSERE O ARGUMENTO NO BUFFER
		buffer.push(argument);
		//SINALIZA QUE HÁ MENOS UM ESPAÇO VAZIO NO BUFFER
		BUFFER_FULL++;
		BUFFER_EMPTY--;
		//CASO A THREAD DESPACHANTE ESTEJA BLOQUEADA PORQUE O BUFFER ESTÁ VAZIO A ACORDA
		pthread_cond_signal(&buffer_empty_cond);
		//SAI DA REGIÃO CRÍTICA
	pthread_mutex_unlock(&buffer_mutex);
	
	//RETORNA O ID PARA QUE O USUÁRIO POSSA PEGAR O RESULTADO DA REQUISIÇÃO
	return id;
}

//PEGA O RESULTADO DA REQUISIÇÃO A PARTIR DO ID DADO PELO USUÁRIO
int pegarResultadoExecucao(int id){
	int result;
	//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEL (VETOR "RESULT")
	pthread_mutex_lock(&result_mutex);
		//ENQUANTO O RESULTADO AINDA NÃO ESTIVER NO VETOR "RESULT" BLOQUEIA O USUÁRIO
		while(RESULT[id] == -1)
			pthread_cond_wait(&result_cond[id], &result_mutex);
		//PEGA O RESULTADO QUANDO DISPONÍVEL
		result = RESULT[id];
		//SETA QUE A POSIÇÃO DO VETOR "RESULT" DADA PELO ID ESTÁ DISPONÍVEL
		RESULT[id] = -1;
		//ENTRA NA REGIÃO CRÍTICA (VETOR "available_result_id)
		pthread_mutex_lock(&available_result_id_mutex);
			//SETA QUE O ID ESTÁ DISPONÍVEL PARA SER USADO
			available_result_id[id] = true;
			//SINALIZA QUE HÁ MAIS UM ESPAÇO LIVRE EM RESULT
			RESULT_FULL --;
			RESULT_EMPTY ++;
			//ACORDA O USUÁRIO CASO ELE ESTEJA ESPERANDO QUE UM ID FIQUE DISPONÍVEL
			pthread_cond_signal(&available_result_id_cond);
		//SAI DA REGIÃO CRÍTICA (VETOR "available_result_id)
		pthread_mutex_unlock(&available_result_id_mutex);
	//SAI DA REGIÃO CRÍTICA (VETOR "RESULT")
	pthread_mutex_unlock(&result_mutex);
	//RETORNA O RESULTADO
	return result;

}


//THREAD RESPONSÁVEL POR GERENCIAR AS THREADS DISPONÍVEIS
void *despachante(void *ignore){
	int bug = 0;
	//LOOP INFINITO
	while(true){
		cout<<"despachante"<<endl;
		ARGUMENT *argument = new ARGUMENT;
		
		int threadId = -1;
		//ENTRA NA REGIÃO CRÍTICA SE DISPONÍVEL (VETOR "BUFFER")
		pthread_mutex_lock(&buffer_mutex);
			//ENQUANTO O BUFFER ESTIVER VAZIO BLOQUEIA A THREAD DESPACHANTE
			while(BUFFER_EMPTY == BUFFER_SIZE)
				pthread_cond_wait(&buffer_empty_cond, &buffer_mutex);
			//PEGA O ARGUMENTO NA FRENTE DA FILA
			*argument = buffer.front();
			//REMOVE O ARGUMENTO DA FILA
			buffer.pop();
			//SINALIZA QUE TEM MAIS UM ESPAÇO VAZIO NO BUFFER
			BUFFER_FULL--;
			BUFFER_EMPTY++;
			//CASO O USUÁRIO ESTEJA BLOQUEADO PELO BUFFER ESTAR CHEIO O ACORDA
			pthread_cond_signal(&buffer_full_cond);
		pthread_mutex_unlock(&buffer_mutex);
		
		//ENTRA NA REGIÃO CRÍTCA SE DISPONÍVEL (VETOR "available_threads")
		pthread_mutex_lock(&available_threads_mutex);
			bool isFree = false;
			//CASO A QUANTIDADE DE THREADS EXECUTANDO SEJA A MÁXIMA BLOQUEIA A THREAD ATÉ QUE ALGUMA THREAD TERMINE SUA EXECUÇÃO
			while(THREADS_EXECUTING == N)
				pthread_cond_wait(&available_threads_cond, &available_threads_mutex);
			//PROCURA POR QUAL THREAD ESTÁ DISPONÍVEL
			for(int i=0; i<N && !isFree; i++){
				//SE A THREAD ESTÁ DISPONÍVEL
				if(available_threads[i]){
					//PEGA O ID DESTA THREAD
					threadId = i;
					//ENCERRA A THREAD
					pthread_cancel(threads[i]);
					//SETA A THREAD COMO INDISPONÍVEL
					available_threads[i] = false;
					//SETA O ID DA THREAD NA STRUCT "argument"
					(*argument).thread_id = i;
					//ENCERRA O LOOP
					isFree = true;
				}
			
			}
			//SINALIZA QUE HÁ MAIS UMA THREAD EXECUTANDO
			THREADS_EXECUTING++;
		//SAI DA REGIÃO CRÍTICA
		pthread_mutex_unlock(&available_threads_mutex);
		//CRIA A THREAD PARA EXECUTAR A FUNÇÃO DADA, tendo como argumento a variável "argument"
		pthread_create(&threads[threadId],NULL,(*argument).function,(void *)argument);
	}
}




int main(){
	init(); //INICIALIZA AS ESTRUTURAS NECESSÁRIAS
	cout<<endl;
	pthread_t despachante_thread;
	ARGUMENT *argument = new ARGUMENT;
	//CRIA A THREAD DESPACHANTE
	pthread_create(&despachante_thread,NULL,despachante,NULL);
	
	//TESTES
	for(int i =0; i<10; i++){
		cout<<agendarExecucao(ARGUMENT(funexec,10))<<endl;
		//cout<<"A"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<pegarResultadoExecucao(i)<<endl;
		//cout<<"B"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<agendarExecucao(ARGUMENT(funexec,10))<<endl;
		//cout<<"C"<<endl;
	}
	for(int i =0; i<10; i++){
		cout<<pegarResultadoExecucao(i)<<endl;
		//cout<<"D"<<endl;
	}


	delete argument;
	pthread_exit(NULL);

}

