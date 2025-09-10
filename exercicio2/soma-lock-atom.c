#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LOOP_LEN 100000 // tamanho do loop de cada thread
#define DIV 1000        // valor para os logs

long int soma = 0; //variavel compartilhada entre as threads
pthread_cond_t cond;
pthread_mutex_t mutex; //variavel de lock para exclusao mutua
int flag_imprima = 1;
int flag_seguir = 0;

//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<LOOP_LEN; i++) {
    //--entrada na SC
    pthread_mutex_lock(&mutex);
    while (flag_imprima) {
        pthread_cond_wait(&cond, &mutex);
        if (!flag_imprima) break;
    }
    soma++;
    if (soma % DIV == 0) flag_imprima = 1;
    pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  int *p_nthreads = (int *) args;
  long last = -1,
       soma_max = LOOP_LEN * *p_nthreads;

  printf("Extra : esta executando...\n");
  while (1) {
    pthread_mutex_lock(&mutex);
    if (!(soma%DIV) && last != soma) { //imprime se 'soma' for multiplo de 1000
        printf("soma = %ld \n", soma);
        last = soma;
        flag_imprima = 0;
        pthread_cond_broadcast(&cond);
    }
    if (soma >= soma_max) {
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
  }
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
  pthread_t *tid; //identificadores das threads no sistema
  int nthreads; //qtde de threads (passada linha de comando)

  //--le e avalia os parametros de entrada
  if(argc<2) {
    printf("Digite: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);

  //--aloca as estruturas
  tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
  if(tid==NULL) {puts("ERRO--malloc"); return 2;}

  //--inicilaiza o mutex (lock de exclusao mutua)
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  //--cria as threads
  for(long int t=0; t<nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //--cria thread de log
  if (pthread_create(&tid[nthreads], NULL, extra, &nthreads)) {
    printf("--ERRO: pthread_create()\n"); exit(-1);
  }

  //--espera todas as threads terminarem
  for (int t=0; t<nthreads+1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("--ERRO: pthread_join() \n"); exit(-1);
    }
  }

  //--finaliza o mutex
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);

  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}
