#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

#define matrixwid 2 //Largura da matriz esparsa (e.g.: 3x2 tem matrixwid = 2)
#define matrixhei 2 //Altura da matriz esparsa (e.g.: 3x2 tem matrixhei = 3)

#define vetsize 2

#define matrix2wid 3
#define matrix2hei 3

#define matrixfullwid 2
#define matrixfullhei 2

struct espDouble{
    double pos = -1;
    double val = 0.0;
};

double respA[vetsize] = {0};

//matriz principal
espDouble matrixvet[matrixhei][matrixwid] =
        {{{0, 2.0}},
         {{1, 1.0}}};

espDouble matrix2vet[matrix2hei][matrix2wid] =
        {{{0, 2.0}},
         {{0, 1.0}, {2, 5.0}},
         {{2, 2.0}}};

double vet[vetsize] = {2, 2};

double matrixfull[matrixfullhei][matrixfullwid] =
        {{1, 1},
        {1, 1}};

void orgMat(espDouble mvet[matrixhei][matrixwid]){ //A MATRIZ ESPARSA DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE
    for (int i = 0; i < matrixhei; i++)
        for (int j = 0; j < matrixwid; j++)
        {
            if (mvet[i][j].pos != -1 && mvet[i][j].pos != j)
            {
               int tempj = j - 1;
               for (j = matrixwid - 2; j > tempj; j--)
               {
                   mvet[i][j + 1].pos = mvet[i][j].pos;
                   mvet[i][j + 1].val = mvet[i][j].val;
               }
                mvet[i][j + 1].pos = -1;
                mvet[i][j + 1].val = 0.0;
               j = tempj + 1;
            }
        }
}


void *threadVetMult(void *tid){

    int i,k;
    long  threadId = (*(long *)tid);
    i = threadId;

	cout << "test " << i << endl;

    for(k=0;k<vetsize;k++)
    {
        respA[i] = respA[i] + matrixvet[i][k].val * vet[k];
    }

    cout << "Fim da thread " << threadId << endl;
    pthread_exit(NULL);
}

void vetMult(){
    orgMat(matrixvet);
    pthread_t matline[matrixhei];
    long *threadids[vetsize];

    for(int tn=0; tn<vetsize; tn++){
        cout << "Criando thread de id = " << tn << endl;
	threadids[tn] = (long *) malloc(sizeof(long)); *threadids[tn] = tn;
        pthread_create(&matline[tn],NULL, threadVetMult, (void *)threadids[tn]);
    }

    for(int u=0; u < vetsize;u++) {
        long *res;
        pthread_join(matline[u], NULL);
    }   

    for (int i = 0; i < vetsize; i++)
            cout << i << "," << 0 << " " << " val: " << respA[i] << endl;
    return;
}

void matEspMult(){
    return;
}

void matMult(){
    return;
}

int main(int argc, char *argv[]){
    char choose;

    retry:
    cin >> choose;

    switch(choose)
    {
        case '1': case 'a': case 'A': vetMult(); break;
        case '2': case 'b': case 'B': matEspMult(); break;
        case '3': case 'c': case 'C': matMult(); break;
        default: goto retry;
    }

    return 0;
}
