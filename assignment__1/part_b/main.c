/*

	FINISHED ON: 15/4/24
	MADE BY:
		Itay Andre Aknin 322959412 איתי אנדרי אקנין
		Roi Israel Forer 322226952 רועי ישראל פורר

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define MAX_STUDENTS 30
#define MAX_STUDENT_NAME 50

void handleError(int error_code);
char** loadLinesToBuffer(int fd, int* num_of_students);
void freeAll(char** config, int config_len, char** inputs, int inputs_len, char** studentNames, int numOfStudents);

int main(int argc, char** argv) {

	// handle config.txt start
	int config_fd = open(argv[1], O_RDONLY);
	handleError(config_fd);

	char** config = NULL;
	int config_len = 0;
	config = loadLinesToBuffer(config_fd, &config_len);

	close(config_fd);

	char* dir_name = config[0];
	char* in_file_name = config[1];
	char* ex_out_file_name = config[2];
	// handle config.txt end


	// load inputs to compare with student
	int input_fd = open(in_file_name, O_RDONLY);
	handleError(input_fd);

	char** inputs = NULL;
	int inputs_len = 0;
	inputs = loadLinesToBuffer(input_fd, &inputs_len);

	close(input_fd);
	// end load inputs

	char** studentNames = NULL;
	int numOfStudents = 0;

	//load student names to a file
	int pid;
	int status;
	if((pid = fork()) == 0) {

		int fd = open("students_list.txt", (O_RDWR | O_CREAT | O_TRUNC), 0666);
		handleError(fd);

		close(1);
		dup(fd);
		close(fd);

		char* args[] = {"ls", "all_students", NULL};
		if(execvp("ls", args) < 0 ) {
			perror(NULL);
			exit(-1);
		}
	}
	handleError(pid);
	// wait for students_list creation.
	wait(&status);

	// load student names into memory
	int fd = open("students_list.txt", O_RDONLY);
	handleError(fd);
	studentNames  = loadLinesToBuffer(fd, &numOfStudents);
	close(fd);
	//for(int i = 0 ; i < numOfStudents; i++) printf("student: %s , id: %d\n", namesArray[i], i);

	int* grades = (int*)calloc(numOfStudents, sizeof(int));

	// Calculate Students Grade:
	for(int i = 0 ; i < numOfStudents; i++) {
		// load students' main
		if((pid = fork()) == 0) {

			int fd = open("student_output.txt", (O_RDWR | O_CREAT | O_TRUNC), 0666);
			handleError(fd);

			close(1);
			dup(fd);
			close(fd);

			char exec_path[128];
			sprintf(exec_path, "%s/%s/main.exe", dir_name, studentNames[i]);
			char* exec_args[] = {exec_path, inputs[0], inputs[1], NULL};

			//printf("%s\n%s", exec_path, exec_args);

			if(execvp(exec_path, exec_args) < 0){
				perror(NULL);
				exit(-1);
			}
		}
		handleError(pid);
		// wait for ouput of student's program
		wait(&status);


		// compare outputs
		if((pid = fork()) == 0) {

			char* exec_path = "../assignment_1/comp.exe";
			char* exec_args[] = {exec_path, "../part_b/student_output.txt", "../part_b/expected_output.txt"};
			if(execvp(exec_path, exec_args) < 0 ) {

				perror(NULL);
				exit(-1);
			}
		}
		handleError(pid);
		wait(&status);

		//grades[i] = WEXITSTATUS(status) == 2 ? 100 : 0;
		grades[i] = WEXITSTATUS(status);
	}

	// updates grades of students
	for(int j = 0; j < numOfStudents; j++) {
		if(grades[j] == 2) grades[j] = 100;
		else
			grades[j] = 0;
	}


	// write to grades.csv
	int grades_fd = open("grades.csv", (O_WRONLY | O_CREAT | O_TRUNC), 0666);
	handleError(grades_fd);

	for(int i = 0 ; i < numOfStudents; i++) {
		//printf("%d\n", grades[i]);

		char buffer[128];
		sprintf(buffer, "%s's grade is: %d\n", studentNames[i], grades[i]);

		if(write(fd, buffer, strlen(buffer)) < 0) {
			perror(NULL);
			exit(-1);
		}
	}

	close(grades_fd);

	freeAll(config, config_len, inputs, inputs_len, studentNames, numOfStudents);
	return 0;
}



void handleError(int error_code) {

	if(error_code < 0) {
		perror(NULL);
		exit(-1);
	}
}

void freeAll(char** config, int config_len, char** inputs, int inputs_len, char** studentNames, int numOfStudents) {

	// free config buffer
	for(int i = 0 ; i < config_len; i++) {

		free(config[i]);
		config[i] = NULL;
	}
	free(config);
	config = NULL;

	// free inputs buffer
	for(int i = 0 ; i < inputs_len; i++) {

		free(inputs[i]);
		inputs[i] = NULL;
	}
	free(inputs);

	// free studentNames
	for(int i = 0; i < numOfStudents; i++) {
		free(studentNames[i]);
		studentNames[i] = NULL;
	}
	free(studentNames);


}


char** loadLinesToBuffer(int fd, int* num_of_students) {

	handleError(fd);

	char** namesArray = NULL;
	char buffer[MAX_STUDENTS][MAX_STUDENT_NAME] = {0};
	char current_char = '\0';
	int i = 0;
	int numOfStudents = 0;

	int bytesRead = -1;
	do {
		bytesRead = read(fd, &current_char, 1);
		//if error then handle it.
		handleError(bytesRead);

		if(bytesRead  <= 0) break;

		if(current_char == '\n') {

			buffer[numOfStudents][i] = '\0';
			numOfStudents++;

			i = 0;
			continue;
		}
		buffer[numOfStudents][i] = current_char;
		i++;

	} while(bytesRead > 0);


	namesArray = (char**)calloc(numOfStudents, sizeof(char*));

	// allocate memory for each student name exact size
	for(int i = 0; i < numOfStudents; i++) {

		namesArray[i] = (char*)calloc(strlen(buffer[i])+1, sizeof(char));
		strcpy(namesArray[i], buffer[i]);

		//namesArray[i][strlen(buffer[i])+1] = '\0';
	}

	*num_of_students = numOfStudents;
	return namesArray;
}
