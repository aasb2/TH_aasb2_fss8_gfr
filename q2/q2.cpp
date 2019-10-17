#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;


//STRUCT QUE SERÁ USADA PARA SALVAR AS INFORMAÇÕES DO USUÁRIO
struct USER{
	string name;
	int id;
	int lastAccess;
	double score;
	double inactivity;
	USER(){
		this->id = -1;
		this->lastAccess = -1;
		this->score = -1;
		this->inactivity = -1;
	}
	USER(int name, int id, int lastAccess, double score, double inactivity){
		this->name = name;
		this->id = 	id;
		this->lastAccess = lastAccess;
		this->score = score;
		this->inactivity = inactivity;
	}
};

//VETOR REPONSÁVEL POR VERIFICAR SE UM ARQUIVO ESTÁ DISPONÍVEL PARA SER ACESSADO (haverá no vetor a quantidade de posições iguais a quantidade de espaços livres)
//SE O ARQUIVO ESTÁ SENDO ACESSADO, OU JÁ FOI ACESSADO POR UMA THREAD O VALOR DA POSIÇÃO DO VETOR EQUIVALENTE AO ARQUIVO QUE ESTÁ SENDO LIDO SERÁ SETADO PARA "true"
//O VETOR TEM TODOS SEUS ELEMENTOS INICIALIZADOS COMO FALSO
bool *access;
//VETOR RESPONSÁVEL POR SALVAR OS ARQUIVOS
vector<fstream > files;
//VETOR RESPONSÁVEL POR SALVAR OS DADOS DE CADA USUÁRIO ENQUANTO CADA ARQUIVO ESTÁ SENDO LIDO
vector<vector<USER>>user;
//VARIÁVEL QUE ARMAZENARÁ A MÉDIA DE INATIVIDADE
double media = 0;
//VARIÁVEL QUE ARMAZENARÁ A QUANTIDADE DE USUÁRIOS
int usersQtd = 0;
//MUTEX QUE PERMITIRÁ A EXCLUSÃO MÚTUA AO VETOR ACCESS
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

//"THREAD" RESPONSÁVEL POR ACESSAR O ARQUIVO
void *access_file(void* nothing){
	
	bool hasFiles = true;
	USER person;
	int file = -1;
	bool isAvailable = false;

	//VERIFICA SE AINDA HÁ ARQUIVOS DISPONÍVEIS PARA SER ACESSADOS
	while(hasFiles){
		file = -1;
		isAvailable = false;
		//PROCURA NO VETOR access SE AINDA HÁ ALGUM ARQUIVO DISPONÍVEL PARA SER ACESSADO
		for(int i=0; i<files.size()&& !isAvailable; i++){
				//ENTRA NA REGIÃO CRÍTICA
				pthread_mutex_lock(&fileMutex);
				//VERIFICA SE O ARQUIVO i ESTÁ DISPONÍVEL PARA SER ACESSADO
				if(!access[i]){
					//CASO ESTEJA DISPONÍVEL ENCERRA O LOOP
					isAvailable = true;
					//SELECIONA O ARQUIVO
					file = i;
					//SETA QUE O ARQUIVO I JÁ ESTÁ SENDO ACESSADO POR UMA THREAD PARA QUE HAJA EXCLUSÃO MÚTUA
					access[i] = true;
				}
				//SAI DA REGIÃO CRÍTICA
				pthread_mutex_unlock(&fileMutex);
		}
		
		//CASO NÃO HAJA MAIS ARQUIVOS DISPONÍVEIS PARA SER ACESSADOS ENCERRA O LOOP
		if(!isAvailable){
			hasFiles = false;
			break;
		}

		
		int line = 0;
		
		//ACESSA OS ARQUIVOS LINHA POR LINHA, PRIMEIRO SALVANDO OS VALORES EM UMA VARIÁVEL DO TIPO USER E DEPOIS SALVANDO NO VETOR "user"
		while(files.at(file)>>person.name>>person.id>>person.lastAccess>>person.score)
			user.at(file).push_back(person);
	}
	//ENCERRA A THREAD
	pthread_exit(NULL);
}


void print(const USER & user){
	cout<<user.name<<" "<<user.id<<" "<<user.lastAccess<<" "<<user.score<<endl;
}

//FUNÇÃO RESPONSÁVEL POR CALCULAR A MÉDIA DE INATIVIDADE DO USUÁRIO
void calculateMedia(){
	double parcialSum = 0;
	double top, bot;
	//CALCULA A MÉDIA DE INATIVIDADE BASEADO NO QUE A QUESTÃO PEDE
	for(int i=0; i<user.size();i++){
		for(int j=0; j<user.at(i).size(); j++){
			usersQtd++;
			top = user.at(i).at(j).lastAccess;
			bot = user.at(i).at(j).score*user.at(i).at(j).score;
			parcialSum += top/bot;
		}
	}
	media = parcialSum/(double)usersQtd;

}

void removeUsers(){
	string fileName;
	double top, bot;
	int lineReal, lineLog;
	for(int i=0; i<user.size();i++){
		//SALVA O NOME DO ARQUIVO EM UMA STRING
		fileName = "banco"+to_string(i+1)+".txt";
		//SETA O CURSOR PARA O INÍCIO DO ARQUIVO QUE SERÁ ACESSADO
		files.at(i).clear();
		files.at(i).seekg(0,ios::beg);
		
		//CALCULA A MÉDIA DE CADA USUÁRIO E O REMOVE CASO ELE ESTEJA DUAS VEZES ACIMA DA MÉDIA
		for(int j=0; j<user.at(i).size(); j++){
			//SETA O CURSOR PARA O INÍCIO DO ARQUIVO
			files.at(i).clear();
			files.at(i).seekg(0,ios::beg);
			top = user.at(i).at(j).lastAccess;
			bot = user.at(i).at(j).score*user.at(i).at(j).score;
			
			//SE MÉDIA DE INATIVIDADE DO USUÁRIO FOR DUAS VEZES MAIOR QUE A MÉDIA REMOVE O USUÁRIO DO ARQUIVO
			if(top/bot >= 2*media){
				//cout<<"TRUE"<<endl;
				lineReal = 0;
				lineLog = 0;
				//VETOR QUE SALVARÁ OS DADOS DOS USUÁRIOS QUE NÂO SERÃO REMOVIDOS
				vector<string> text;
				string line;
				//IMPRIME NA TELA O USUÁRIO INATIVO
				print(user.at(i).at(j));
				int findUserLine = 0, id;
				//SETA O CURSOR PARA O INÍCIO DO ARQUIVO
				files.at(i).clear();
				files.at(i).seekg(0,ios::beg);

				//LOCALIZA EM QUE LINHA ESTÁ O USUÁRIO INATIVO
				while(files.at(i)>>line>>id){
					if(user.at(i).at(j).name == line && user.at(i).at(j).id == id)
							break;
					getline(files.at(i),line);
					findUserLine++;
				}
				//SETA O CURSOR PARA O INÍCIO DO ARQUIVO
				files.at(i).clear();
				files.at(i).seekg(0,ios::beg);
				//SALVA O CONTEÚDO DO ARQUIVO (COM EXCEÇÃO DA LINHA COM O USUÁRIO A SER REMOVIDO) NO VETOR DE STRINGS "text"
				for(int k =0; k<user.at(i).size(); k++){
					getline(files.at(i),line);
					if(k!=findUserLine)
						text.push_back(line);
					lineReal++;
				}
				//FECHA O ARQUIVO
				files.at(i).close();
				//DELETA O ARQUIVO
				remove(fileName.c_str());
				//CRIA UM NOVO ARQUIVO COM O MESMO NOME DO ARQUIVO QUE ESTÁ SENDO ANALISADO
				ofstream temp(fileName);
				//FECHA O ARQUIVO
				temp.close();
				//ABRE O ARQUIVO NOVAMENTE
				files.at(i).open(fileName);
				//INSERE NO NOVO ARQUIVO OS USUÁRIOS QUE NÃO FORAM REMOVIDOS
				for(int k=0; k<lineReal-1; k++){
					files.at(i)<<text.at(k)<<endl;
				}

				
			}
		}
	}
}

int main(){
	int N, A, T;
	cin>>N>>A>>T;
	vector<pthread_t> threadid(T);
	//SETA O TAMANHO DO VETOR PARA O TAMANHO DADO
	user.resize(A);
	//ALOCA O ARRAY DE BOOLEANOS 0 E O INICIALIZA COM O VALOR ZERO
	access = new bool[A]();
	for(int i=0; i<A; i++)
		access[i] = false;
	//"INSERE" OS ARQUIVOS EM UM "VETOR DE ARQUIVOS"
	for(int i=0; i<A; i++){
		files.push_back(fstream("banco"+to_string(i+1)+".txt"));
		//cout<<"banco"+to_string(i+1)+".txt"<<endl;
	}
	
	//CRIA AS THREADS QUE ABRIRÃO OS ARQUIVOS E IRÃO SALVAR OS DADOS DE CADA USUÁRIO
	//DEPOIS DISSO ESPERA QUE ELAS TERMINEM
	for(int i=0; i<T; i++){
		pthread_create(&threadid.at(i),NULL,access_file,NULL);
		pthread_join(threadid.at(i),NULL);
	}
	//CALCULA A MÉDIA GERAL DE INATIVIDADE
	calculateMedia();
	//REMOVE OS USUÁRIOS INATIVOS
	removeUsers();
	//FECHA OS ARQUIVOS QUE FORAM ABERTOS
	for(int i=0; i<A; i++){
		files.at(i).close();
	}
	delete access;
	//ENCERRA A THREAD MAIN
	pthread_exit(NULL);
	

}
