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
vector<pthread_mutex_t> mutexes;
pthread_mutex fileMutex = PTHREAD_MUTEX_INITIALIZER;

void *access_file(void* nothing){
	bool hasFiles = true;
	while(hasFiles){
		int file;
		bool isAvailable = false;
		for(int i=0; i<A&& !isAvailable; i++){
				pthread_mutex_lock(fileMutex);
				if(access[i]
				pthread_mutex_unlock(fileMutex);
		}
		pthread_mutex_lock()

	}
	pthread_exit(NULL);
}

int main(){
	int N, A, T;
	cin>>N>>A>>T;
	vector<pthread_t> threadid(T);
	mutexes.resize(A);
	for(int i =0; i<A; i++)
		mutexes.at(i) = PTHREAD_MUTEX_INITIALIZER;
	access = new bool[A]();
	
	for(int i=0; i<A; i++)
		files.push_back(fstream("banco"+to_string(i+1)+".txt"));
	for(int i=0; i<T; i++){
		if(pthread_create(&threadid.at(i),NULL,access_file,NULL))
			exit(-1);
		pthread_join(threadid.at(i),NULL);
	}
	
		
	

}
