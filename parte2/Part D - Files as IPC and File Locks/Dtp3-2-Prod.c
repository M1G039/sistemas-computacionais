#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

struct flock prod_lock;

void *numbersProducer(void *vargp)
{
    unsigned int number;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        number = rand() % 1000;
        printf("Generated number %u\n", number);
        // Prepare data to be written to file (integer to fixed-size string format)
        snprintf(buffer, BUFFER_SIZE, "%09d\n", number);

        // We nedd to implement lock
        lock.l_tupe = F_WRLCK;
        if(fcntl(file_descriptor, F_SETLKW, &lock) < 0)
        {
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(-1);
        }

        // Here lock was acquired, you can write to the file
        // Write to file
        if (write(file_descriptor, buffer, strlen(buffer)) == -1) {
            fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
        }

        // Release the lock
        lock.l_type = F_UNLCK;
        if (fcntl(file_descriptor, F_SETLK, &lock) < 0)
        {
            exit(-1);
        }

        sleep(1);
    }

    return NULL;
}

int main()
{
    int file_descriptor = -1;
    pthread_t tid_1;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, append data (don't overwrite file)
    // - set permissions to 0644 (owner can read/write, group and others can read)
    file_descriptor = open(PRODUCER_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (file_descriptor == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    // Add the file descriptor to the thread
    pthread_create(&tid_1, NULL, numbersProducer, (void *)&file_descriptor);

    pthread_join(tid_1, NULL);

    exit(0);
}
