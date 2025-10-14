#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM 100000
#define MAX_NCONSUMERS 10 // como primalidade é CPU-bound, não faz sentido
                          // ter mais consumidores do que núcleos no processador

// variáveis globais
int *channel,       // canal a ser usado entre produtores e consumidores
    N, M;           // valores de entrada

// variáveis globais de controle
sem_t canalVazio,   // Começa com 1 sinal
      slotCheio,    // Começa com 0 sinais
      mutex;        // exclusão mútua de acesso ao canal

// Estrutura de dados para gravar resultados
typedef struct _Results {
  int total_prime_count;
  int winners_prime_count;
  int winners_count;
  int *winners_ids;
} Results;

//---------------------------- DETECTOR DE PRIMOS ------------------
int ehPrimo(long long int n) {
  int i;
  if (n <= 1)
    return 0;
  if (n == 2)
    return 1;
  if (n % 2 == 0)
    return 0;
  for (i = 3; i < sqrt(n) + 1; i += 2)
    if (n % i == 0)
      return 0;
  return 1;
}
//----------------------------- RETIRA -----------------------------
int remove_int() {
  static int out = 0;
  sem_wait(&slotCheio);
  sem_wait(&mutex);
  int elem = channel[out];
  out = (out + 1) % M;
  if (out == 0)
    sem_post(&canalVazio);
  sem_post(&mutex);
  return elem;
}
// ---------------------------- PRODUTOR---------------------------------
void *int_prod(void * args) {
  int value;
  for (int i = 0; i < N; i++) {
    value = rand() % MAX_NUM;     // produz um novo valor
    sem_wait(&mutex);
    channel[i % M] = value;       // insere no canal
    sem_post(&mutex);
    if (i % M == M - 1) {
      for (int j = 0; j < M; j++) // libera os consumidores
        sem_post(&slotCheio);
      sem_wait(&canalVazio);      // aguarda até o canal ficar vazio
    }
  }
  for (int i = 0; i < M; i++) {   // preenche o canal com um poison pill
    channel[i] = -1;
  }
  for (int j = 0; j < M; j++)     // libera os consumidores
    sem_post(&slotCheio);
  pthread_exit(NULL);
}
//----------------------------- CONSUMIDOR -----------------------------
void *int_cons(void *args) {
  int elem,
      *prime_counter = (int*) malloc(sizeof(int));
  *prime_counter = 0;
  while (1) {
    elem = remove_int();
    if (elem == -1) {           // recebeu um poison pill. Hora de parar
      pthread_exit(prime_counter);
    } else if (ehPrimo(elem)) {
      *prime_counter += 1;
    }
  }
}
//----------------------------- MAIN -----------------------------
int main(int argc, char *argv[]) {
  pthread_t *tids;
  int *thread_prime_count;
  Results result;

  while ((fscanf(stdin, "%d,%d", &N, &M)) == 2) {
    // incialização dos argumentos
    if (N < M) {
      fprintf(stderr, "Argumento N deve ser maior que M\n");
      return 1;
    }
    printf("lidos M=%d, N=%d\n", M, N);

    M = M <= MAX_NCONSUMERS ? M : MAX_NCONSUMERS;

    // criação do vetor de threads: M consumidores + 1 produtor
    tids = (pthread_t *)malloc(sizeof(pthread_t) * (M + 1));

    // alocação do canal
    channel = (int *)malloc(sizeof(int) * M);

    // inicialização dos semáforos
    sem_init(&canalVazio, 0, 0);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutex, 0, 1);

    // inicialização do produtor
    pthread_create(tids, NULL, int_prod, NULL);

    // incialização dos consumidores
    for (int i = 1; i < M+1; i++)
      pthread_create(&tids[i], NULL, int_cons, NULL);

    result.winners_ids= (int *)malloc(sizeof(int)*N);
    result.total_prime_count = result.winners_prime_count =
      result.winners_count = 0;

    // finalização das threads
    for (short i = 1; i < M+1; i++) {
      pthread_join(tids[i], (void **)&thread_prime_count);

      printf("thread %d contou %d primos\n", i, *thread_prime_count);
      // atualizando contagem de primos
      result.total_prime_count += *thread_prime_count;

      if (*thread_prime_count == result.winners_prime_count) {
        // contagem de primos foi igual a feita pelos vencedores presentes
        result.winners_ids[result.winners_count] = i;
        result.winners_count++;
      } else if (*thread_prime_count > result.winners_prime_count) {
        // contagem de primos foi maior do que a feita pelos antigos vencedores
        result.winners_ids[0] = i;
        result.winners_count = 1;
        result.winners_prime_count = *thread_prime_count;
      }
      free(thread_prime_count);
    }

    // aguardar o produtor finalizar
    pthread_join(tids[0], NULL);

    // Resultados
    printf("Total de Primos contados: %d\n", result.total_prime_count);
    printf("Total de primos contados pela(s) thread(s) vencedora(s): %d\n",
           result.winners_prime_count);
    printf("IDs dos vencedores: ");
    for (int i = 0; i < result.winners_count; i++) {
      printf("%d ", result.winners_ids[i]);
    }
    printf("\n");
    // Finalizações
    sem_destroy(&slotCheio);
    sem_destroy(&canalVazio);
    sem_destroy(&mutex);
    free(tids);
    free(channel);
    free(result.winners_ids);
    printf("\n");
  }

  return 0;
}
