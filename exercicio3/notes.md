# Descrição

Uso exclusivo de semáforos para implementação do padrão de
produtor consumidor

- Há uma únia thread produtora.
- O produtor enche o buffer todo.
- As threads consumidores removem elementos do buffer
- As threads, ao remover o elemento, verifica sua primalidade

```c
(long long int n) {
  int i;
  if (n<=1) return 0;
  if (n==2) return 1;
  if (n%2==0) return 0;
  for (i=3; i<sqrt(n)+1; i+=2)
    if(n%i==0) return 0;
  return 1;
}
```

- A saída do programa consiste de: a quantidade de primos encontrados e
a thread que contou mais primos.

# Requsiitos Produtor-Consumidor

Como dito, o problema implementa o padrão podutor consumidor, cujos requisitos
clássicos são:

- Produtor não pode preencher um buffer cheio. Sincronização por condição.

- As threads consumidoras não podem acessar o buffer vazio. Aqui deve ser usada
a sincronização por condição.

- Os elementos são inseridos em FIFO.

- elementos não podem ser sobrescritos.

- um elemento sendo removido por vez. Quando a thread for ler, é preciso evitar
a violação de aotmicidade da leitura.

Um requisito adicinal da aplicação é:

- Enquanto o produtor enche o buffer, nenhuma thread consumidora pode acessá-lo.
Logo é preciso implementar exclusão mútua para acesso ao buffer. Isso porque
o produtor preenche o buffer de uma vez.

# Sobre os canais

O produtor irá preencher um vetor de tamanho $N$, enquanto que o o canal tem
tamanho $M$, além do que $M << N$. Isso permitirá que o produtor esteja
produzindo novas entradas mesmo que o canal esteja sendo lido por consumidores.

Quando o canal ficar vazio, uma operação (que poderá ser feita pelo próprio
produtor ou por uma thread auxiliar) preenchê-lo-á novamente. O produtor volta
a produzir elementos, e os consumidores a consumir.

# Sobre a contagem

Quando houver o join das threads, elas deverão retornar a quantidade de primos
contados por elas. A thread principal irá somar essa quantidade a uma variável
contadora e verificará se aquela foi a thread que contou mais primos até então.

Acredito que essa abordagem seja melhor do que fazer as threads incrementarem
um contador global, pois toda vez que elas fossem alterar o contador, elas
poderiam ficar bloqueadas em vez de fazer outra coisa.

# Sobre o número de threads

Num primeiro momento, faz sentido haver mais threads do que posições
no canal M. Do contrário, os consumidores apenas leriam parte do canal, e
enquanto estivesse executando, o produtor não poderia carregar novos elementos
lá, porque ele ainda não estaria vazio.

Também não faria muito sentido haver muito mais threads do que elementos no
canal, pois é possível que elas fiquem ociosas durante um tempo.
Então para simplificar, o número de threads será igual ao número de elementos
no canal.

# Geração dos casos de teste
Os valores de entrada do programa são $N$ e $M$, logo deverá ser gerado um
arquivo para diferentes tamanhos de de N e M.

# Avaliação de Corretude

Será feita uma implementação sequencial simples do programa para a avaliação
de corretude.



