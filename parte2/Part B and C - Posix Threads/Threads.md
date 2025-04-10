# POSIX Threads

### O que é uma thread?
Ao contrário dos processos, que têm a sua memória isolada, as threads são componentes de um processo e compartilham recursos com o mesmo, e com outras threads que esse mesmo processo possa possuir. 

### Funções importantes
Dentro do `pthread.h` destacam-se as seguintes funções para manipulação de threads:

```C
int main()
{
    // These variables will contain a thread id when they are created by pthread_create
    pthread_t tid_1, tid_2;

    // if a call to pthread_create is successfull a new thread will be created
    // and sheduled for execution immediately, running the function pointed at 
    // by the third patrameter of the function call.
    // One can also pass arguments to it as long as they are a void pointer (only one argument is allowed)
    pthread_create(&tid_1, NULL, myThreadFunction, (void *)(&tid_1) );
    pthread_create(&tid_2, NULL, myThreadFunction, (void *)(&tid_2) );

    // A call to pthread_join only returns if the thread pointed by its first parameter 
    // has been terminated. We do this to make sure the resources associated with the threads
    // have been correctly freed when it finishes its execution. This also means that
    // The main thread will only exit when all other threads are done, ensuring all of
    // them can complete their task.
    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);
    
	exit(0);
}
```
O programa acima cria duas threads que rodam uma função `myThreadFunction` e o programa termina assim que ambas as threads terminarem também. Para mais detalhes sobre cada função é recomendado ler a sua manpage (clica no nome em baixo).

[`pthread_create(4)`:](https://man7.org/linux/man-pages/man3/pthread_create.3.html)

**Parametros:**
***pthread_t tid**: um apontador para o thread id
***attr**: apontador para uma estrutura que contém informação sobre os atributos da nova thread a ser criada. `NULL` usa as configurações default.
***start_routine()**: apontador para a função a ser executada pela thread.
***arg**: argumento a ser passado para a thread