/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

//classe runnable
class MyCallable implements Callable<Long> {
  // construtor
  MyCallable() {
  }

  // método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i = 1; i <= 100; i++) {
      s++;
    }
    return s;
  }
}

class PrimoCallable implements Callable<Boolean> {
  private int num;

  PrimoCallable(int n) {
    this.num = n;
  }

  public Boolean call() throws Exception {
    if (num < 2)
      return false;

    for (int i = 2; i <= Math.sqrt(num); i++) {
      if (num % i == 0)
        return false;
    }
    return true;
  }
}

// classe do método main
public class FutureHello {
  private static final int N = 1000000;
  private static final int NTHREADS = 10;

  public static void main(String[] args) {

    // cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    // cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<Boolean>> list = new ArrayList<Future<Boolean>>();

    for (int i = 1; i < N; i++) {
      Callable<Boolean> worker = new PrimoCallable(i);
      /*
       * submit() permite enviar tarefas Callable ou Runnable e obter um objeto Future
       * para acompanhar o progresso e recuperar o resultado da tarefa
       */
      Future<Boolean> submit = executor.submit(worker);
      list.add(submit);
    }

    // System.out.println(list.size());
    // pode fazer outras tarefas...

    // recupera os resultados e faz o somatório final
    int count = 0;
    for (Future<Boolean> future : list) {
      try {
        if (future.get())
          count++; // bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println("Quantidade de primos entre 1 e " + N + ": " + count);
    executor.shutdown();
  }
}
