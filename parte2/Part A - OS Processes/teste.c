#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("Starting %d\n", getpid());
    fork();
    printf("Running process id: %d\n", getpid());

    return 0;
}

