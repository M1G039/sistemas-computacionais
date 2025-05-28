/***********************
* Test function for pipe device driver.
* It receives:
* -> name of shared file passed as 1st argument
* -> data to write as a string in the 2nd argument

* This function should start two processes/threads: a producer and a consumer.
* Both processes/threads should open the file device /dev/pipe ( hint: use fork()! Or threads... whatever you want! )
* The producer must write to it, and the consumer must read from it.
************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// Error exit
void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

// Producer process/thread. Receives the file descriptor and string to be written.
int producer(int fd, char *str) {

  // Write string to the device and print a message.

  // Print a message to visualize written data
  // printf("Producer wrote data X to file...\n");

  // Debug:
  printf("Returning from producer\n");
}

// Consumer process/thread. Receives the file descriptor. It should read from a file.
int consumer(int fd) {

  // Give time to start the producer.
  sleep(1);

  // Read the characters stored on the device, one at a time.
  
  // Debug:
  printf("Returning from consumer\n");
}

int main(int argc, char *argv[]) {
  
  if( argc < 3 )
	  report_and_exit("Not enough arguments...");

  int file_descriptor = -1;

  // Open file with name given by the first argument argv[1]
  if (file_descriptor < 0)
    report_and_exit("Open failed...");

  // Launch the producer/consumer processes/threads. Don't forget to terminate the processes/threads once they finish.

  return 0;
}