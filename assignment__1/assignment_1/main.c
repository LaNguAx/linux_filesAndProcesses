/*
	MADE BY 
		Itay Andre Aknin 322959412 איתי אנדרי אקנין
		Roi Israel Forer 322226952 רועי ישראל פורר
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 1
int main(int argc, char* argv[]) {

	if(argc-1 != 2) {
		const char* errorText = "This program must be passed 2 files!\n";
		int eID = write(2, errorText, strlen(errorText));
		if(eID < 0) _exit(-2);
		_exit(-1);
	}
	// returns a reference int of argv[1]'s file descriptor id  which is stored in pbc's file descriptor table
	int fd1 = open(argv[1], O_RDONLY);
	// if opening argv[1] failed.
	if(fd1 < 0) { 
		const char* errorMsg = "Error opening file 1!\n";
		int eID = write(2, errorMsg, strlen(errorMsg));
		if(eID < 0) _exit(-2);
		_exit(-1);
	}

	int fd2 = open(argv[2], O_RDONLY);
	if(fd2 < 0) {

		close(fd1);	
		const char* errorMsg = "Error opening file 2!\n";
		int eID = write(2, errorMsg, strlen(errorMsg));
		if(eID < 0) _exit(-2);

		_exit(-1);
	}
	
	char buffer1 = '\0';
	char buffer2 = '\0';

	int bytesRead1 = -1;
	int bytesRead2 = -1;

	do {
		bytesRead1 = read(fd1, &buffer1, BUFFER_SIZE);
		bytesRead2 = read(fd2, &buffer2, BUFFER_SIZE);
		if(bytesRead1 < 0 || bytesRead2 < 0){ 
			close(fd1);
			close(fd2);
			_exit(-1);
		}
		//debug
		//printf("buffer1: %c --- buffer2: %c\n", buffer1, buffer2);

		if(buffer1 != buffer2){
			close(fd1);
			close(fd2);
			_exit(1);
		}
	} while(bytesRead1 == BUFFER_SIZE && bytesRead2 == BUFFER_SIZE);

	close(fd1);
	close(fd2);

	if(buffer1 == buffer2) _exit(2);
}

