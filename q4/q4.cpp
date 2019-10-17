#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;
int prime_numbers_qtd = 0;
int greatest_divisors_qtd = 0;
int number_with_the_greatest_divisors = 0;

int max(int a, int b){
	return a>b ? a : b;
}
int min(int a, int b){
	return a<b ? a : b;
}

//CALCULA A QUANTIDADE DE NÚMEROS PRIMOS EM UM INTERVALO
void *prime_numbers_verificator(void *arg){
	pair<int,int> interval = *((pair<int, int> *) (arg));
	
	for(int i =interval.first; i<=interval.second; i++){ 
		bool isPrime = true;
		int max_prime = ceil(sqrt(i));
		if(i>1){
			for(int j=2; j<=max_prime && isPrime; j++){
				if(i%j == 0 && i!=j)
					isPrime = false;
			}
			if(isPrime)
				prime_numbers_qtd++;
		}
	}
	pthread_exit(NULL);

}

//CALCULA QUAL A MAIOR QUANTIDADE DE DIVISORES NO INTERVALO
void *divisors_verificator(void * arg){
	pair<int,int> interval = *((pair<int, int> *) (arg));
	int current;
	int curMax = -1;
	for(int i = interval.first; i <=interval.second; i++){
		current = 0;
		for(int j=1; j<=i; j++){
			if(i%j == 0)
				current ++;
		}
		curMax = max(current,curMax);
	}
	greatest_divisors_qtd = curMax;
	pthread_exit(NULL);
}

//CALCULA QUAL O NÚMERO NO INTERVALO TEM A MAIOR QUANTIDADE DE DIVISORES
void *number_with_the_most_divisors_verificator(void * arg){
	pair<int,int> interval = *((pair<int, int> *) (arg));
	int current;
	int curMax = -1, curMaxNum = INT_MAX;
	for(int i = interval.first; i <=interval.second; i++){
		current = 0;
		
		for(int j=1; j<=i; j++){
			if(i%j == 0)
				current ++;
		}
		if(current> curMax){
			curMax = current;
			curMaxNum = i;
		}
		else if(current == curMax)
			curMaxNum = min(i,curMaxNum);
		
	}
	number_with_the_greatest_divisors = curMaxNum;
	pthread_exit(NULL);
}


int main(int argc, char *argv[]){
	pthread_t prime;
	pthread_t divisors_qtd;
	pthread_t divisors_num;
	
	pair<int,int> *interval = new pair<int,int>;

	cin>>(*interval).first>>(*interval).second;
	
	int rc;   
	//CRIA AS THREADS QUE FARÃO AS TAREFAS SOLICITADAS
	if(pthread_create(&prime, NULL, prime_numbers_verificator, (void *) interval))
		exit(-1);
	if(pthread_create(&divisors_qtd, NULL, divisors_verificator, (void *) interval))
		exit(-1);
	if(pthread_create(&divisors_num, NULL,number_with_the_most_divisors_verificator, (void *) interval))
		exit(-1);
	
	//ESPERA QUE ESSAS THREADS TERMINEM
	pthread_join(prime,NULL);
	pthread_join(divisors_qtd,NULL);
	pthread_join(divisors_num,NULL);
	//IMPRIME O RESULTADO NA TELA
	cout<<prime_numbers_qtd<<" "<<greatest_divisors_qtd<<" "<<number_with_the_greatest_divisors<<endl;
	
	//ENCERRA A THREAD
	pthread_exit(NULL);
	return 0;
}
