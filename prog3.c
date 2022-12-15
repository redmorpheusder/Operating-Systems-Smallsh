#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h> 






int main() {

	int exitprog = 1;
	char userinput[2048];
	char command[20];
	char* argList[512];
	int status = 0;
	int countwords = 0;
	int len = 0;
	int fgbg = 0;
	int i = 0;
	int j = 0;
	int g = 0;
	int k = 0;
	int isin = 0;
	int isout = 0;
	int pock = 0;
	int pock2 = 0;
	int checkexec = 0;
	int infile = 0;
	int outfile = 0;
	int result = 0;
	int pidcheck = 0;
	int pidstorer = 0;
	int dollarfound = 0;

	char delim[] = " ";
	char pidprint[20];

	do {


		//Take input from user
		printf(": ");

		fgets(userinput, 2048, stdin);

		strtok(userinput, "\n");	//remove \n from the end of a string
		len = strlen(userinput);




	


		char* ptr = strtok(userinput, delim);
		countwords = 0;

		while (ptr != NULL) {                   //Separate words in string and store in ArgList
			argList[countwords] = ptr;
			ptr = strtok(NULL, delim);
			countwords++;
		}


		argList[countwords] = NULL;             //Set null to the end of a string 
		fgbg = 0;

		if (strcmp(argList[countwords - 1], "&") == 0) {       //Check Last Element for Background
			fgbg = 1;
			argList[countwords - 1] = NULL;					   //If Last word is &, Set flag fgbf=1 and Delete &
			countwords--;
		}


		char outputFile[100];
		char inputFile[100];

		i = 0;
		isin = 0;
		isout = 0;
		dollarfound = 0;

		for (i = 0; i < countwords; i++) {                                     //Get input and output file names if entered 
			if (strcmp(argList[i], "<") == 0) {
				argList[i] = NULL; // the index holding the the desginator gets null
				strcpy(inputFile, argList[i + 1]);
				isin = 1;

			}

			if (strcmp(argList[i], ">") == 0) {
				argList[i] = NULL; // the index holding the the desginator gets null
				strcpy(outputFile, argList[i + 1]);
				isout = 1;
			}

			if (strstr(argList[i], "$$") != NULL) {       //Search for $$
				dollarfound = i;

			}
		}
		

		infile = 0;
		outfile = 0;
		result = 0; 
		                           //Opening input or output files if needed.
		





		//If user input nothing or comment starting with '#'
		if ((strcmp(argList[0], "\n") == 0) || (userinput[0] == '#')) {


		}

		//Using 'cd' built in function
		else if (strcmp(argList[0], "cd") == 0) {
			if (countwords == 1) {					// if only cd is called
				chdir(getenv("HOME"));
			}

			else {
				if (chdir(argList[1]) != 0) {
				}
			}

		}





		//Using 'status' built in function
		else if (strcmp(argList[0], "status") == 0) {
			printf("exit value %d\n", status);
		}


		//Exit from shell 
		else if (strcmp(argList[0], "exit") == 0) {
			exitprog = 0;
			exit(status); 
		}


		//Passing argument to fork if input is unkown
		else {

			int childStatus = 0;
			// Fork a new process
			pid_t spawnPid = fork();

			switch (spawnPid) {
			case -1:
				perror("fork()\n");
				exit(1);
				break;
			case 0:
				// In the child process
				// Replace the current program with "/bin/ls"
				if (isin == 1) {
					if (fgbg == 1) {
						infile = open("/dev/null", 0);

						if (infile < 0) {
							perror("error to open input file\n");
							exit(1);
						}
						dup2(infile, 0);
						close(infile);
					}

					else {
						infile = open(inputFile, 0);

						if (infile < 0) {
							perror("error to open input file\n");
							exit(1);
						}
						dup2(infile, 0);
						close(infile);

					}


				}

				if (isout == 1) {
					if (fgbg == 1) {
						outfile = creat("/dev/null", 0644);

						if (outfile < 0) {
							perror("error to open output file\n");
							exit(1);
						}
						dup2(outfile, 1);
						close(outfile);
					}

					else {
						outfile = creat(outputFile, 0644);

						if (outfile < 0) {
							perror("error to open output file\n");
							exit(1);
						}
						dup2(outfile, 1);
						close(outfile);

					}


				}

				checkexec = 0;
				checkexec = execvp(argList[0], argList);
				// exec only returns if there is an error
				perror("execve");
				exit(2);
				break;
			default:
				// In the parent process
				// Wait for child's termination

				if (fgbg == 1) {
					waitpid(spawnPid, &childStatus, WNOHANG); // with no hang and check child
					printf("Background pid: %d\n", spawnPid);

				}

				else {
					pock = 0;
					pock2 = 0;
					waitpid(spawnPid, &childStatus, 0);
					fflush(stdout);

					if (WIFEXITED(childStatus)) {                                             //Checking the status
						pock = WEXITSTATUS(childStatus);						
						
					}

					if (WIFSIGNALED(childStatus)) {                                       
						pock2 = WTERMSIG(childStatus);
						
					}

					if (pock == 0) {
						if (pock2 == 0) {
							status = 0;
						}
					}

					else {
						//printf("Child exited abnormally due to signal %d\n", WTERMSIG(childStatus));
						status = 1;
					}

				}
				pidcheck = waitpid(-1, &childStatus, WNOHANG);

				while (pidcheck > 0) { //Checking if process is done
					printf("background pid %d is done: ", pidcheck);
					status = 0; 
					printf("exit value %d\n", status);
					fflush(stdout);
					pidcheck = 0;
				}


				break;
			}
		}

	} while (exitprog == 1);






	return 0;
}