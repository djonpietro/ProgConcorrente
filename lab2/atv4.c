#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define TOL 1e-3

typedef unsigned long dimension;

typedef struct t_args {
  float *a;
  float *b;
  dimension fatia;
} t_args;


void ler_vetor(float *vet, dimension dim, FILE *fp) {
  size_t c;
  c = fread(vet, sizeof(float), dim, fp);
  if (c != dim) {
    fprintf(stderr, "ERRO - FALHA NA LEITURA DE DADOS DO VETOR\n");
    exit(3);
  }
}

void * produto_interno(void *args) {
  t_args *arg = (t_args*) args;
  double *soma = (double*) malloc(sizeof(double));
  *soma = 0;
  for (dimension i = 0; i < arg->fatia; i++) {
    *soma += *(arg->a + i) * *(arg->b + i);
  }
  free(args);
  pthread_exit((void*) soma);
}

int main(int argc, char *argv[]) {
  // Declaracao de variaveis
  FILE      *fp;                    // stream para o arquivo de dados
  size_t    c;                      // variavel auxiliar para teste na leitura de arquivos
  dimension dim,                    // dimensao do vetor
            fatia;                  // qtde de elementos a serem percorridos pela thread
  double    resultado_teste,        // resultado lido do arquivo de teste
            soma = 0,               // produto interno computado pelo programa
            *resultado_thread;      // ponteiro para o resultado do valor calculado pela thread
  short int nthreads;               // qtde de threads
  int       id_teste = 1,           // indice do caso de teste
            qtde_testes_falhos = 0; // quantos testes falharam

  // verificar a quantidade de argumentos
  if (argc<3) {
    fprintf(stderr, "ERRO - ARGUMENTOS FALTANTES\n");
    exit(1);
  }

  // Abrir a stream do arquivo
  fp = fopen(argv[2], "rb");
  if (!fp) {
    fprintf(stderr, "ERRO - FALHA AO ABRIR O ARQUIVO PARA LEITURA\n");
    exit(2);
  }

  // Inicializar o numero de threads
  nthreads = atoi(argv[1]);
  pthread_t tids[nthreads];

  // ler dimensao do caso de teste
  while ( (c = fread(&dim, sizeof(unsigned long), 1, fp)) == 1) {
    // calcular fatia da thread
    fatia = (dimension) dim / nthreads;

    // ler valores dos vetores e salva-los
    float A[dim], B[dim]; // vetores de dados
    ler_vetor(A, dim, fp); ler_vetor(B, dim, fp);

    // ler valor do produto interno
    if ( (c = fread(&resultado_teste, sizeof(double), 1, fp)) != 1) {
      fprintf(stderr, "ERRO - FALHA NA LEITURA DO RESULTADO DE TESTE %d\n", id_teste);
      exit(3);
    }

    // realizar o produto interno
    for (int i = 0; i < nthreads; i++) {
      // Alocar estrutura dos argumentos
      t_args *args = (t_args *) malloc(sizeof(t_args));
      if (!args) {
        fprintf(stderr, "ERRO - FALHA NA ALOCACAO DOS ARGUMENTOS\n");
        exit(1);
      }
      // preparar os argumentos
      args->a = A + fatia * i;
      args->b = B + fatia * i;
      args->fatia = i == nthreads-1 ? fatia + dim % nthreads : fatia;
      // criar a nova thread
      pthread_create(&tids[i], NULL, produto_interno, (void*) args);
    }
    // verificar se o valor bate
    for (int i = 0; i < nthreads; i++) {
      if (pthread_join(tids[i], (void**) &resultado_thread)) {
        fprintf(stderr, "ERRO - ERRO DE JUNÇÃO DA THREAD\n");
      }
      soma += *resultado_thread;
      free(resultado_thread);
    }

    if (fabs(soma - resultado_teste) > TOL) {
      fprintf(stderr, "TESTE %d FALHOU - RESULTADO TESTE %f - RESULTADO OBTIDO %f\n", id_teste, resultado_teste, soma);
      qtde_testes_falhos++;
    }
    // else {
    //   fprintf(stderr, "TESTE BEM SUCEDIDO\n");
    // }
    // preparar para o próximo caso de teste
    soma = 0; id_teste++;
  }
  if (!feof(fp)) {
    fprintf(stderr, "ERRO - FALHA NA LEITURA DA DIMENSÃO DO VETOR\n");
    exit(1);
  }
  else
    fprintf(stdout, "%d TESTES BEM SUCEDIDOS\nFIM\n", id_teste - 1 - qtde_testes_falhos);

  return 0;
}

