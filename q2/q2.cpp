#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

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


bool *access;
vector<fstream > files;
vector<vector<USER>>user;
double media = 0;
int usersQtd = 0;
//vector<pthread_mutex_t> mutexes;
pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;
void *access_file(void* nothing){
	//cout<<"IT WORKS"<<endl;
	bool hasFiles = true;
	USER person;
	int file = -1;
	bool isAvailable = false;
	while(hasFiles){
		file = -1;
		isAvailable = false;
		for(int i=0; i<files.size()&& !isAvailable; i++){
				pthread_mutex_lock(&fileMutex);
				if(!access[i]){
					isAvailable = true;
					file = i;
					access[i] = true;
				}
				pthread_mutex_unlock(&fileMutex);
		}
		//cout<<"file " + to_string(file) +" is avaliable"<<endl; 
		if(!isAvailable){
			hasFiles = false;
			break;
		}

		
		int line = 0;
		while(files.at(file)>>person.name>>person.id>>person.lastAccess>>person.score)
			user.at(file).push_back(person);
	}

	pthread_exit(NULL);
}


void print(const USER & user){
	cout<<user.name<<" "<<user.id<<" "<<user.lastAccess<<" "<<user.score<<endl;
}


void calculateMedia(){
	double parcialSum = 0;
	double top, bot;
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
		fileName = "banco"+to_string(i+1)+".txt";
		files.at(i).clear();
		files.at(i).seekg(0,ios::beg);
		for(int j=0; j<user.at(i).size(); j++){
			files.at(i).clear();
			files.at(i).seekg(0,ios::beg);
			top = user.at(i).at(j).lastAccess;
			bot = user.at(i).at(j).score*user.at(i).at(j).score;
			//cout<<media<<"/"<<top/bot<<endl;
			//cout<<i<<"-"<<j<<endl;
			if(top/bot >= media){
				cout<<"TRUE"<<endl;
				lineReal = 0;
				lineLog = 0;
				vector<string> text;
				string line;
				print(user.at(i).at(j));
				int findUserLine = 0, id;
				files.at(i).clear();
				files.at(i).seekg(0,ios::beg);
				while(files.at(i)>>line>>id){
					if(user.at(i).at(j).name == line && user.at(i).at(j).id == id)
							break;
					getline(files.at(i),line);
					findUserLine++;
				}

				files.at(i).clear();
				files.at(i).seekg(0,ios::beg);
				for(int k =0; k<user.at(i).size(); k++){
					getline(files.at(i),line);
					if(k!=findUserLine)
						text.push_back(line);
					lineReal++;
				}
				files.at(i).close();
				remove(fileName.c_str());
				ofstream temp(fileName);
				temp.close();
				files.at(i).open(fileName);
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
	user.resize(A);
	access = new bool[A]();
	for(int i=0; i<A; i++)
		access[i] = false;
	for(int i=0; i<A; i++){
		files.push_back(fstream("banco"+to_string(i+1)+".txt"));
		//cout<<"banco"+to_string(i+1)+".txt"<<endl;
	}

	for(int i=0; i<T; i++){
		pthread_create(&threadid.at(i),NULL,access_file,NULL);
		pthread_join(threadid.at(i),NULL);
	}
	
	calculateMedia();
	removeUsers();
	for(int i=0; i<A; i++){
		files.at(i).close();
		//cout<<"banco"+to_string(i+1)+".txt"<<endl;
	}
	
	pthread_exit(NULL);
	

}