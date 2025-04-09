# OS Processes
---
## O que é um processo?
Num sistema operativo, um processo é uma instância de um programa que contêm o seu próprio código e memória associada. Quando temos um ambiente com vários processos a decorrer, estes coexistem competindo por tempo de CPU e outros recursos de hardware, especialmente I/O.

Nos sistema POSIX podemos usar a system call `fork()` para clonar o 'parent process'. O resultado é uma cópia do processo original ao qual se chama 'child process'. Neste processo, toda a memória do processo original é copiada, tornando o child process completamente independente. Ficamos assim com duas cópias do programa original a correr em paralelo, completamente autónomas uma da outra!

No seguinte programa:
```C
#include <stdio.h>
#include <unistd.h>

int main()
{
    // We start with the parent process
    printf("Starting with process id: %d\n", getpid());
    // Invoke fork to clone the parent process
    fork();
    // From this moment onwards, two independent processes are running,
    // with different PIDs
    printf("Running process id: %d\n", getpid());

    return 0;
}
```
Podemos observar no terminal o resultado, por exmplo
```
Starting with process id: 1890
Running process id: 1890
Running process id: 1891
```
o que nos indica que de facto foram criados dois processos com PIDs distintos.

Outro aspeto importante da função `fork()` é o seu valor de retorno. Consultando a sua [manpage](https://man7.org/linux/man-pages/man2/fork.2.html), esta função retorna valores diferentes consoante o processo. Em caso de sucesso a clonar o processo retorna o PID do child process no parent process e retorna 0 no child process. No caso de falha retorna -1 para o parent e nehum child é criado.

`wait()`:

A função [wait()](https://man7.org/linux/man-pages/man2/wait.2.html) serve para interromper a execução do código até ser detetada uma mudança no estado de um child process - child terminated, child stopped by signal or child resumed by signal. Retorna o PID da child que foi terminada ou -1 em caso de falha.