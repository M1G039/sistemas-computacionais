#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "Dtp3.h"

void *oddNumbersConsumer(void *vargp)
{
    int prod_file_descriptor = *(int *)vargp;
    int file_descriptor_odd;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644
    file_descriptor_odd = open(PRODUCER_FILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_descriptor_odd == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", ODD_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.

        read_size = read(prod_file_descriptor, buffer, BUFFER_SIZE);
        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 != 0)
            {
                printf("Storing odd number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);

                // TODO - Store formated odd number into the odd numbers file
                write(file_descriptor_odd, buffer, BUFFER_SIZE);
            }
        }
        usleep(250000);
    }

    return NULL;
}

void *evenNumbersConsumer(void *vargp)
{
    int file_descriptor = *(int *)vargp;
    int file_descriptor_even;
    unsigned int number;
    char buffer[BUFFER_SIZE];
    int read_size;

    // TODO - Open the even numbers file so that:
    // - only write-only operations are allowed
    // - file is created if not existing
    // - if file exists, overwrite existing data
    // - set permissions to 0644

    if (file_descriptor_even == -1)
    {
        fprintf(stderr, "Error: %s: %s\n", EVEN_NUMBERS_FILE_NAME, strerror(errno));
        return NULL;
    }

    while (1)
    {
        // TODO - Read one number from the producers file. Recall
        // that numbers are in text format with 9 characters.

        if (read_size > 0)
        {   // Convert text format (string) to integer
            number = atoi(buffer);
            if (number % 2 == 0)
            {
                printf("Storing even number %u\n", number);
                snprintf(buffer, BUFFER_SIZE, "%09d\n", number);
                
                // TODO - Store formated even number into the even numbers file
            }
        }
        usleep(250000);
    }

    return NULL;
}

int main()
{
    pthread_t tid_1, tid_2;

    // TODO - Open the producer file so that:
    // - only read operations are allowed
    // - file is created if not existing
    // - set permissions to 0644
    int file_descriptor = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT, 0644);
    if (file_descriptor = -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    // Because threades will share the access to the file, if we use the same file descriptor
    // for both threads, the file cursor will be moved by both threads and we will not be able
    // to read the file correctly. We need to create a new file descriptor for each thread.

    int file_descriptor_clone = open(PRODUCER_FILE_NAME, O_RDONLY | O_CREAT, 0644);
    if(file_descriptor_clone = -1)
    {
        fprintf(stderr, "Error: %s: %s\n", PRODUCER_FILE_NAME, strerror(errno));
        return -1;
    }

    pthread_create(&tid_1, NULL, oddNumbersConsumer, (void *)file_descriptor);
    pthread_create(&tid_2, NULL, evenNumbersConsumer, (void *)file_descriptor_clone);

    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);

    exit(0);
}
