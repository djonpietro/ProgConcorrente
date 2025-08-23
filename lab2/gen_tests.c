#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX 10000
#define MIN -1000
#define MAX_DIM 100000

void save_vet(float *vet, unsigned long dim, FILE *fp) {
  size_t c;
  if ( (c=fwrite(vet, sizeof(float), dim, fp)) != dim ) {
    fprintf(stderr, "ERRO - FALHA AO SALVAR O VETOR NO ARQUIVO");
    exit(2);
  }
}

void create_test(FILE *fp) {
  // randomizar a dimensão do vetor
  unsigned long dim = 1 + ((float)rand() / RAND_MAX) * (MAX_DIM-1);
  size_t c;

  // Escrever a dimensao dim no arquivo
  if ( (c=fwrite(&dim, sizeof(unsigned long), 1, fp)) != 1) {
    fprintf(stderr, "ERRO - FALHA AO ESCREVER DIMENSAO NO ARQUIVO\n");
    exit(1);
  }

  fprintf(stdout, "Dim: %lu\n", dim);

  float A[dim], B[dim];
  double s = 0;

  // Preencher os vetores e calcular o produto interno
  for (unsigned long i = 0 ; i < dim ; i++) {
    A[i] = MIN + ( ((float) rand()) / RAND_MAX ) * (MAX - MIN);
    B[i] = MIN + ( ((float) rand()) / RAND_MAX ) * (MAX - MIN);
    s += A[i] * B[i];
  }

  save_vet(A, dim, fp); save_vet(B, dim, fp);
  if ( (c=fwrite(&s, sizeof(double), 1, fp)) != 1) {
    fprintf(stderr, "ERRO - FALHA AO ESCREVER PRODUTO INTERNO NO ARQUIVO\n");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  FILE *fp;

  // Abrindo o arquivo de teste
  if ( (fp=fopen("tests.dat", "wb")) == NULL ) {
    fprintf(stderr, "ERRO - O ARQUIVO DE TESTE NAO PODE SER GERADO\n");
    exit(1);
  }

  if (argc<2) {
    fprintf(stderr, "ERRO - NUMERO DE TESTES NAO INFORMADO\n");
    exit(1);
  }

  int n = atoi(argv[1]);
  for (int i = 0; i < n; i++)
    create_test(fp);
  return 0;
}
