#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "p5_helper.h"

#define BUFFER_MAX_SIZE 4

// Create mutex
pthread_mutex_t mutex;

// This is the shared data space.
// We need to protect it to prevent access from multiple threads at the same time.
unsigned int data_space[BUFFER_MAX_SIZE];

// Circular buffer struct.
circ_buff_t buffer = { 
    data_space,
    BUFFER_MAX_SIZE,
    0,
    0
};

void* producer(void *arg)
{
    unsigned int push_value;
    while (1) {
        push_value = rand() % 1000;
        // Lock mutex
        if (p_thread_mutex_lock(&mutex) == 0) {
            // Now we can access the shared data space safely.
            if (circ_buff_push(&buffer, push_value) == 0)
            {
                printf("Producer: %u\n", push_value);
                // Notify the consumer that there is data available
                pthread_cond_signal(&buffer_prod_condv);
            }
        }
    }
    
    return NULL;
}

void* consumer(void *arg)
{
    unsigned int pop_value;
    while (1) {
        // Lock mutex
        
        if (p_thread_mutex_lock(&mutex) == 0) {
            // Now we can access the shared data space safely.
            while (circ_buff_isempty(&buffer)) {
                pthread_cond_wait(&buffer_prod_condv, &mutex);
            }


        }
    }
    
    return NULL;
}
int main(void)
{
    //Seeding...
	srand(time(NULL));

    pthread_t tid[2];

    // Initialize the mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    
	pthread_create(&(tid[0]), NULL, &producer, NULL);
	pthread_create(&(tid[1]), NULL, &consumer, NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}