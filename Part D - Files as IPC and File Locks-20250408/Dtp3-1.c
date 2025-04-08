#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define READ_BUFFER_SIZE 64
#define MIN_NUMBER_OF_ARGUMENTS 2

static char buffer[READ_BUFFER_SIZE];

int get_file_size(int file_descriptor);
void display_file_content(int file_descriptor);

int main(int argc, char **argv)
{
	char *file_name;
	int file_descriptor;

	if (argc < MIN_NUMBER_OF_ARGUMENTS){
    	fprintf(stderr, "Error: usage: %s filename\n", argv[0]);
    	return -1;
	}
	
	file_name = argv[1];

	file_descriptor = open(file_name, O_RDONLY);
	if (file_descriptor == -1){
		fprintf(stderr, "Error: %s: %s\n", file_name, strerror(errno));
    	return -1;
	}
  
	fprintf(stdout, "File '%s' opened (file descriptor with value %d)\n", file_name, file_descriptor);
	fprintf(stdout, "Opened file at offset: %lld\n", lseek(file_descriptor, 0, SEEK_CUR));
	fprintf(stdout, "File size: %d\n", get_file_size(file_descriptor));
	fprintf(stdout, "File contents:\n");
	display_file_content(file_descriptor);

	close(file_descriptor);

	return 0;
}

int get_file_size(int file_descriptor){
	int file_size = 0;

	// We can use lseek() to send the file cursor to the end of the file.
	// That position will be the number of bytes the file has.
	file_size = lseek(file_descriptor, 0, SEEK_END);
	if (file_size == -1){
		fprintf(stderr, "Error: %s\nCould not reach the end of the file.", strerror(errno));
		return -1;
	}

	// Be carefull that after this operation if we want to read the file we have to 
	// set the file cursor back to the end of the file.
	lseek(file_descriptor, 0, SEEK_SET);

	return file_size;
}

void display_file_content(int file_descriptor){
    int read_size;

	// This will loop until there are no bytes left to read from the file.
	// We are using a 64 byte buffer, menaing we are reading 64 bytes from the file at
	// a time, printing them to the terminal and repeating this process until all
	// the file has been printed.
    fprintf(stdout, "\n------------------------------------------------------------------------------\n");
	while ((read_size = read(file_descriptor, buffer, READ_BUFFER_SIZE)) > 0){
        // Write to the standard output (terminal).
		write(STDOUT_FILENO, buffer, read_size);
	}
    fprintf(stdout, "\n------------------------------------------------------------------------------\n");
	
    return;
}
