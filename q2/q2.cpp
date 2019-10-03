#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;
bool *access;

void *access_file(void* file){



}

int main(){
	int N, A, T;
	cin>>N>>A>>T;
	vector<fstream *> files(A);
	files accessed
	access = new bool[A];
	for(int i=0; i<A; i++){
		files.at(i) = new fstream;
		(*(files.at(i))).open("banco"+to_string(i+1)+".txt");
	}
	
	

}
