#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

//Sonikku
/*      i.
      _ | ) )
    (  )| |/
     \|||||
      ||||    OK!
      ||||
*/

/*INSTRUÇÃO DE USO:
1. Defina o tamanho das matrizes nos defines;
2. Defina os elementos das matrizes esparsas na parte do código que as inicia, de dois possível modos:
    a. (Mais confiável) Na thread main, defina as posições manualmente, colocando do seguinte formato:
        matrizesparsa[(linha)][(t)].pos = (posição na matriz);
        matrizesparsa[(linha)][(t)].val = (valor do elemento);
        Onde o valor de t inicia em 0 para cada linha diferente, e para cada novo elemento na mesma linha, adiciona-se +1 ao valor de t;
        Os valores de posição na mesma linha devem ser representados em ordem crescente. Caso não, sua printagem no final não será correta.

        Ex.:

        | 2 0 3 |
        | 0 0 0 |
        | 0 0 -1| é representado:

        #define largura 3
        #define altura 3

        matrizesparsa[0][0].pos = 0;
        matrizesparsa[0][0].val = 2.0;
        matrizesparsa[0][1].pos = 2;
        matrizesparsa[0][1].val = 3.0;
        matrizesparsa[2][0].pos = 2;
        matrizesparsa[2][0].val = -1.0;

    b. (Não funciona em todo compilador) Durante a inicialização das matrizes, represente os elementos nesse formato:
    matrizesparsa[largura][altura] = {
        { {pos00, val}, {pos01, val}, {pos02, val} },  ||LINHA N. 1
        { {pos 10, val} },                             ||LINHA N. 2
        { {-1, 0} },                                   ||LINHA N. 3
        { {pos 30, val}, {pos31, val} }                ||LINHA N. 4
    };
    Onde:
        - pos00 < pos01 < pos02 (Elementos da mesma linha devem ser representados em ordem crescente. Caso não, sua printagem no final não será correta);
        - pos30 != pos31 (dois elementos da mesma linha não podem ter a mesma posição);
        - { {-1, 0} } representa uma linha vazia. Talvez funcione como {} ao invés de {-1, 0};

    Ex.:

    | 2 0 3 |
    | 0 0 0 |
    | 0 0 -1| é representado:

    #define largura 3
    #define altura 3

    matrizesparsa[largura][altura] = {
        { {0, 2.0}, {2, 3} },
        { {-1, 0} },                                   ||(OU { {} },)
        { {2, -1} }
    };

3. Defina a outra matriz esparsa da mesma maneira, ou defina o vetor/matriz denso(a).
    a. Vetor:
        - Definido no formato {val0, val1, val2, val3, ..., valn} tal que val0 é o valor da posição 0, val1 é o valor da posição 1, etc.;
        - Defina o tamanho do vetor em vetsize antes.
        Ex.:

        O vetor:
        |1 |
        |-3|
        |10|
        |0 |

        É definido por:

        #define vetsize 4

        vet[vetsize] = {1, -3, 10, 0};

    b. Matriz densa:
        - Definido no formato:
        {
            {val00, val01, val02, ..., val0n},
            {val10, val11, val12, ..., val1n},
            .
            .
            .
            {valm0, valm1, valm2, ..., valmn}
        }
        Tal que val00 é o valor da linha 0, coluna 0, val01 é o valor da linha 0, coluna 1, val 12 é o valor da linha 1, coluna 2, etc.;
        - O tamanho da segunda matriz esparsa e da matriz densa ambos são definidos em matrix2hei (altura) e matrix2wid (largura).

        Ex.:

        A matriz densa:

        | 3   1  0 |
        | -4 -1 -10|
        | 0   5  8 |

        É definida por:

        #define matrix2hei 3
        #define matrix2wid 3

        matrixfull[matrix2hei][matrix2wid] = {
            {3, 1, 0},
            {-4, -1, -10},
            {0, 5, 8}
        };

4. Finalmente, após definir todas as matrizes/vetores, o código pode ser executado, e uma operação diferente é executada baseado no caractere que for enviado:
    - Se "1/a/A" for enviado, ocorrerá a multiplicação da matriz esparsa com o vetor denso, que printa, em ordem:
        = Inicialização e encerramento das threads;
        = Matriz resultante;
        = Matriz esparsa;
        = Vetor denso.
    - Se "2/b/B" for enviado, ocorrerá a multiplicação da matriz esparsa com a segunda matriz esparsa, que printa, em ordem:
        = Inicialização e encerramento das threads;
        = Matriz resultante;
        = Matriz esparsa;
        = Segunda matriz esparsa;
        = A transposta da segunda matriz esparsa (usada no cálculo).
    - Se "3/c/C" for enviado, ocorrerá a multiplicação da matriz esparsa com a matriz densa, que printa, em ordem:
        = Inicialização e encerramento das threads;
        = Matriz resultante;
        = Matriz esparsa;
        = Matriz densa.

    4.1. Caso não possa ocorrer a multiplicação no caso selecionado, não há a criação de threads novas e apenas há a printagem de uma mensagem de erro.
*/

//O CÓDIGO USA MATRIZES ESTÁTICAS, LOGO É NECESSÁRIO COLOCAR O TAMANHO DAS MATRIZES NAS DEFINIÇÕES ANTES DE USÁ-LO.

//-----------------------------/DEFINES/------------------------------
#define matrixhei 2 //Altura da matriz esparsa (e.g.: 3x2 tem matrixhei = 3)
#define matrixwid 2 //Largura da matriz esparsa (e.g.: 3x2 tem matrixwid = 2)

#define vetsize 2 //Qtd de elementos no vetor

#define matrix2hei 2 //Altura da segunda matriz (esparsa ou não)
#define matrix2wid 2 //Largura da segunda matriz (esparsa ou não)
//--------------------------------------------------------------------

struct espDouble{  //Estrutura representando um elemento de uma matriz esparsa
    double pos = -1;
    double val = 0.0;
};

double respA[matrixhei] = {0}; //Matriz para a resposta do caso A (matriz esparsa * vetor denso)

double respB[matrixhei][matrix2wid] = {0}; //Matriz para as respostas dos casos B e C (matriz esparsa * outras matrizes)

//Matriz Esparsa Principal (DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE PARA PRINTAGEM)
espDouble matrixvet[matrixhei][matrixwid];

//Matriz Esparsa Secundária (DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE PARA PRINTAGEM)
espDouble matrix2vet[matrix2hei][matrix2wid];

//Matriz Esparsa Transposta (NÃO DEVE SER INICIADA COM ELEMENTOS, DEIXE VAZIA)
espDouble matrix2vetT[matrix2wid][matrix2hei];

//Vetor Denso
double vet[vetsize] = {2, 3};

//Matriz Densa
double matrixfull[matrix2hei][matrix2wid] = {{1, 1}, {1, 1}};


//-----------------------------FUNÇÕES DE ORGANIZAÇÃO PARA PRINTAGEM-----------------------------
//Efetivamente, essas funções transformam a matriz esparsa em uma matriz de formato normal, para facilitar a printagem.
void orgMat2T(espDouble mvet[matrix2wid][matrix2hei]){ //A MATRIZ ESPARSA SECUNDÁRIA DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE
    for (int i = 0; i < matrix2wid; i++) //Percorre-se as linhas da matriz inteira, porém...
    {
        if (mvet[i][0].pos == -1) //...caso o primeiro elemento tiver pos -1, significa que não existem elementos não-zero na linha, então pula para a próxima.
            continue;
        for (int j = 0; j < matrix2hei; j++) //Percorre-se as colunas da matriz inteira, porém...
        {
            if (mvet[i][j].pos == -1) //...se a posição for -1, não há mais elementos não-zero na linha, então ele da break no loop do j e vai para a próxima linha.
                break;
            else if (mvet[i][j].pos != j) //Se a posição for j (não necessita mudar), ele não organiza.
            {
               int tempj = j - 1; //Salva a posição logo antes de j, para garantir que voltará para organizar o resto.
               for (j = matrix2hei - 2; j > tempj; j--) //Começa do final da matriz e vai empurrando para a direita os elementos até chegar na posição antiga (por isso é necessário a ordem crescente).
               {
                   mvet[i][j + 1].pos = mvet[i][j].pos;
                   mvet[i][j + 1].val = mvet[i][j].val;
               }
                mvet[i][j + 1].pos = -1; //Define o elemento inicialmente empurrado como 0
                mvet[i][j + 1].val = 0.0;
                j = tempj + 1; //Retorna para a posição de j.
            }
        }
    }
}

//Os códigos de organização depois desse são iguais, mas para as matrizes diferentes.

void orgMat2(espDouble mvet[matrix2hei][matrix2wid]){ //A MATRIZ ESPARSA SECUNDÁRIA DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE
    for (int i = 0; i < matrix2hei; i++)
    {
        if (mvet[i][0].pos == -1)
            continue;
        for (int j = 0; j < matrix2wid; j++)
        {
            if (mvet[i][j].pos == -1)
                break;
            else if (mvet[i][j].pos != -1 && mvet[i][j].pos != j)
            {
               int tempj = j - 1;
               for (j = matrix2wid - 2; j > tempj; j--)
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
}

void orgMat(espDouble mvet[matrixhei][matrixwid]){ //A MATRIZ ESPARSA DEVE ESTAR REPRESENTADA EM ORDEM CRESCENTE
    for (int i = 0; i < matrixhei; i++)
    {
        if (mvet[i][0].pos == -1)
            continue;
        for (int j = 0; j < matrixwid; j++)
        {
            if (mvet[i][j].pos == -1)
                break;
            else if (mvet[i][j].pos != -1 && mvet[i][j].pos != j)
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
}

//------------------------------------------------------------------------------------------------

//--------------------------------FUNÇÕES DE MODIFICAÇÃO DE MATRIZES------------------------------
//Realiza a transposição da matriz 2:
void transPMat2(){
    int t[matrix2wid] = {0}; //Define um índice das linhas da transposta como 0, para saber quantos elementos esparsos estão presentes em cada linha atualmente.
    for (int i = 0; i < matrix2hei; i++) //Percorre todas as linhas da matriz esparsa 2, mas...
    {
        if (matrix2vet[i][0].pos == -1) //...se o primeiro elemento tiver pos -1, significa que não existem elementos não-zero na linha, então pula para a próxima.
                continue;
        for (int j = 0; j < matrix2wid; j++) //Percorre todas as colunas da matriz esparsa 2, mas...
        {
            if (matrix2vet[i][j].pos == -1) //...se achar um elemento de pos -1, significa que não existem mais elementos não-zero nessa linha, então pula para a próxima.
                break;
            matrix2vetT[(int)((matrix2wid - 1) - matrix2vet[i][j].pos)][t[(int)((matrix2wid - 1) - matrix2vet[i][j].pos)]].pos = i; //(matrix2wid - 1) - matrix2vet[i][j].pos equivale a posição de linha desse valor na matriz transposta. t[] desse valor só indica quantos elementos já estão lá.
            matrix2vetT[(int)((matrix2wid - 1) - matrix2vet[i][j].pos)][t[(int)((matrix2wid - 1) - matrix2vet[i][j].pos)]].val = matrix2vet[i][j].val;
            t[(int)((matrix2wid - 1) - matrix2vet[i][j].pos)]++; //Aumenta a quantidade de elementos na linha que foi trocada.
        }
    }
}

//Converte a matriz densa em uma matriz esparsa, permitindo o reuso do caso B para o caso C:
void convMat(){
    for (int i = 0; i < matrix2hei; i++) //Percorre todas as linhas da matriz densa
    {
        int t = 0; //Define a quantidade de elementos esparsos da linha atual como 0
        for (int j = 0; j < matrix2wid; j++) //Percorre todas as colunas da matriz densa 
        {
            if (matrixfull[i][j] != 0) //Se achar um elemento não-zero...
            {
                matrix2vet[i][t].pos = j; //...coloca ele na matriz esparsa, com pos igual a sua coluna atual.
                matrix2vet[i][t].val = matrixfull[i][j];
                t++; //Aumenta a quantidade de elementos esparsos na linha atual.
            }
        }
    }
}
//------------------------------------------------------------------------------------------------

//--------------------------------------FUNÇÕES DE THREADS----------------------------------------
//Função para o caso A, multiplicação da matriz esparsa pelo vetor denso:
void *threadVetMult(void *tid){

    int i, k;
    long  threadId = (*(long *)tid);
    i = threadId; //A linha calculada é o ID da thread.

    if (matrixvet[i][0].pos == -1) //Se o primeiro elemento da linha tiver pos -1, não há elementos não-zero nessa linha, logo...
    {
        cout << "Linha vazia, fim da thread " << threadId << endl; //...não há necessidade de calcular ela, então encerra-se a thread imediatamente.
        pthread_exit(NULL);
    }

    //Caso contrário, calculamos o elemento dessa linha da thread. Como o vetor só possui largura 1, só temos a soma para um elemento. Assim:

    for(k=0;k < matrixwid; k++) //Percorre todos os elementos dessa linha da matriz esparsa.
    {
        if (matrixvet[i][k].pos == -1) //Se achar um elemento com pos -1, não há elementos não-zero restantes, então pulamos para o encerramento da thread.
            break;
        respA[i] = respA[i] + matrixvet[i][k].val * vet[(int)(matrixvet[i][k].pos)]; //Soma da multiplicação dos elementos da matriz com os elementos correspondentes no vetor.
    }

    cout << "Fim da thread " << threadId << endl; //Fim da thread.
    pthread_exit(NULL);
}

//Função para o caso B e C, multiplicação da matriz esparsa por outra matriz, densa ou esparsa:
void *threadMatEspMult(void *tid){
    int i = 0;
    long  threadId = (*(long *)tid);
    i = threadId; //A linha calculada é o ID da thread.

    if (matrixvet[i][0].pos == -1) //Se o primeiro elemento da linha tiver pos -1, não há elementos não-zero nessa linha, logo...
    {
        cout << "Linha vazia, fim da thread " << threadId << endl; //...não há necessidade de calcular ela, então encerra-se a thread imediatamente.
        pthread_exit(NULL);
    }

    for (int k = 0; k < matrix2wid; k++) //Percorre a quantidade de elementos da linha da matriz resultante
    {
        for (int t = 0; t < matrixwid; t++) //Percorre todos os elementos dessa linha da matriz esparsa.
        {
            if (matrixvet[i][t].pos == -1) //Se não existirem mais elementos não-zero na linha, pulamos para o cálculo do próximo elemento da matriz resultante.
                break;
            for (int p = 0; p < matrix2hei; p++) //Caso contrário, percorremos a transposta da segunda matriz para achar se existe algum elemento a com mesma posição do elemento atual.
            {
                if (matrix2vetT[i][p].pos == -1) //Caso não tenham mais elementos não-zero nessa linha da segunda matriz, pulamos para o próximo elemento da matriz esparsa.
                    break;
                if(matrixvet[i][t].pos == matrix2vetT[(int)((matrix2wid - 1) - k)][p].pos) //Caso ache dois elementos com a mesma posição na mesma linha, calculamos o número a ser somado:
                    respB[i][k] = respB[i][k] + matrixvet[i][t].val * matrix2vetT[(int)((matrix2wid - 1) - k)][p].val; //Soma da multiplicação dos elementos da matriz, com mesma posição, entre si.
            }
        }
    }

    cout << "Fim da thread " << threadId << endl; //Fim da thread.
    pthread_exit(NULL);
}
//------------------------------------------------------------------------------------------------

//-----------------------------------FUNÇÕES PARA CADA CASO---------------------------------------
//Função do caso A:
void vetMult(){
    if (vetsize != matrixwid) //Checa para ver se a multiplicação pode ocorrer.
    {
        cout << "A matriz não pode ser multiplicada por esse vetor." << endl;
        return;
    }
    pthread_t matline[matrixhei]; //Se sim, criamos um vetor de todas as threads,
    long *threadids[matrixhei]; //e um outro com seus respectivos IDs.

    for(int tn=0; tn < matrixhei; tn++){ //Ocorre a criação de cada thread, com IDs progressivamente maiores, igual a quantidade de linhas da matriz esparsa.
        cout << "Criando thread de id " << tn << endl;
	    threadids[tn] = (long *) malloc(sizeof(long)); *threadids[tn] = tn;
        pthread_create(&matline[tn],NULL, threadVetMult, (void *)threadids[tn]); //Thread criada recebe seu ID, para saber qual linha deve-se calcular.
    }

    for(int u=0; u < matrixhei;u++) { //pthread_join esperando todas as threads calcularem seus resultados para poder ocorrer a printagem.
        long *res;
        pthread_join(matline[u], NULL);
    }

    //Printagem dos resultados:
    
    cout << endl << endl << endl;

    cout << "-----------------------------------------------------" << endl << "MATRIZ RESULTANTE:" << endl;

    for (int i = 0; i < matrixhei; i++)
            cout << "| " << respA[i] << "|" << endl;

    cout << "-----------------------------------------------------" << endl << "MATRIZ ESPARSA:" << endl;


    for (int i = 0; i < matrixhei; i++)
    {
        cout << "| ";
        for (int j = 0; j < matrixwid; j++)
            cout << matrixvet[i][j].val << " ";   
        cout << "|" << endl;
    }

    cout << "-----------------------------------------------------" << endl << "VETOR:" << endl;
    cout << "| ";
    for (int i = 0; i < vetsize; i++)
    {
        cout << vet[i] << " ";
    }
    cout << "|" << endl;

    cout << "-----------------------------------------------------" << endl;
    return;
}

//Função do caso B:
void matrixEspMult(bool type){ //Esta função possui um bool para ver se é o caso B ou C, pois a printagem final varia dependendo de qual for.
    if (matrix2hei != matrixwid) //Checa para ver se a multiplicação pode ocorrer.
    {
        cout << "A matriz não pode ser multiplicada por esse vetor." << endl;
        return;
    }
    transPMat2(); //Se sim, ocorre a transposição da segunda matriz,
    pthread_t matline[matrixhei]; //a criação de um vetor com todas as threads,
    long *threadids[matrixhei]; //e outro vetor com seus respectivos IDs.

    for(int tn=0; tn < matrixhei; tn++){ //Ocorre a criação de cada thread, com IDs progressivamente maiores, igual a quantidade de linhas da matriz esparsa.
        cout << "Criando thread de id " << tn << endl;
	    threadids[tn] = (long *) malloc(sizeof(long)); *threadids[tn] = tn; //Thread criada recebe seu ID, para saber qual linha deve-se calcular.
        pthread_create(&matline[tn], NULL, threadMatEspMult, (void *)threadids[tn]);
    }

    for(int u=0; u < matrixhei;u++) { //pthread_join esperando todas as threads calcularem seus resultados para poder ocorrer a printagem.
        long *res;
        pthread_join(matline[u], NULL);
    }

    //Printagem dos resultados:

    cout << endl << endl << endl;

    cout << "-----------------------------------------------------" << endl << "MATRIZ RESULTANTE:" << endl;

    for (int i = 0; i < matrixhei; i++)
    {
        cout << "| ";
        for (int j = 0; j < matrix2wid; j++)
            cout << respB[i][j] << " ";   
        cout << "|" << endl;
    }

    cout << "-----------------------------------------------------" << endl << "MATRIZ ESPARSA:" << endl;

    orgMat(matrixvet);
    for (int i = 0; i < matrixhei; i++)
    {
        cout << "| ";
        for (int j = 0; j < matrixwid; j++)
            cout << matrixvet[i][j].val << " ";   
        cout << "|" << endl;
    }
    if (type == 0) //Se for do caso B (0, false), printa a matriz esparsa.
    {
        cout << "-----------------------------------------------------" << endl  << "MATRIZ ESPARSA N. 2:" << endl;
        orgMat2(matrix2vet);
        for (int i = 0; i < matrix2hei; i++)
        {
            cout << "| ";
            for (int j = 0; j < matrix2wid; j++)
                cout << matrix2vet[i][j].val << " ";   
            cout << "|" << endl;
        }
    }
    else //Caso contrário, printa a matriz densa.
    {
        cout << "-----------------------------------------------------" << endl  << "MATRIZ DENSA:" << endl;
        for (int i = 0; i < matrix2hei; i++)
        {
            cout << "| ";
            for (int j = 0; j < matrix2wid; j++)
                cout << matrixfull[i][j] << " ";   
            cout << "|" << endl;
        }
    }

    if (type == 0) //Se for do caso B (0, false), também printa a transposta da matriz esparsa.
    {
        cout << "-----------------------------------------------------" << endl  << "MATRIZ ESPARSA N.2 TRANSPOSTA:" << endl;

        orgMat2T(matrix2vetT);
        for (int i = 0; i < matrix2wid; i++)
        {
            cout << "| ";
            for (int j = 0; j < matrix2hei; j++)
                cout << matrix2vetT[i][j].val << " ";   
            cout << "|" << endl;
        }
    }

    return;
}
//------------------------------------------------------------------------------------------------

//--------------------------------------FUNÇÕES PRINCIPAIS----------------------------------------

void matMult(){ //Função para o caso C, que
    convMat(); //converte a matriz densa em esparsa, e
    matrixEspMult(1); //envia para o cálculo do caso B, mas com tipo C (1, true), para a printagem.
    return; //retorna para a main após o cálculo.
}

int main(int argc, char *argv[]){
    char choose; //Char para definir a opção escolhida.

//---DEFINA AS MATRIZES AQUI CASO O FORMATO b. NÃO FUNCIONE. MAIS INFORMAÇÃO NO COMEÇO DO CÓDIGO---
    matrixvet[0][0].val = 2;
    matrixvet[0][0].pos = 1;
    matrixvet[1][0].val = -1;
    matrixvet[1][0].pos = 0;

    matrix2vet[0][0].val = 1;
    matrix2vet[0][0].pos = 0;
    matrix2vet[1][0].val = 1;
    matrix2vet[1][0].pos = 1;
//--------------------------------------------------------------------------------------------------

    retry: //Caso o usuário envie algo que não seja 1/a/A, 2/b/B ou 3/c/C, volta-se para ler outro char até isso ocorrer.
    cin >> choose;

    switch(choose)
    {
        case '1': case 'a': case 'A': vetMult(); break; //Caso 1/a/A
        case '2': case 'b': case 'B': matrixEspMult(0); break; //Caso 2/b/B
        case '3': case 'c': case 'C': matMult(); break; //Caso 3/c/C
        default: goto retry; //Goto retorna para a interrupção de lida, nos casos não-definidos.
    }

    return 0; //Fim do código.
}
