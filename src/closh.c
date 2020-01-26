// closh.c - COSC 315, Winter 2020
// Group 31 (Ryan Pinkney, Lawrence Fritzler, Jeff Thomson)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

// tokenize the command string into arguments - do not modify
void readCmdTokens(char* cmd, char** cmdTokens) {
	cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
	int i = 0;
	cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
	while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
		cmdTokens[i] = strtok(NULL, " ");
	}
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
	char c = getchar();
	while (getchar() != '\n');
	return c;
}

// main method - program entry point
int main() {
	char cmd[81]; // array of chars (a string)
	char* cmdTokens[20]; // array of strings
	int count; // number of times to execute command
	int parallel; // whether to run in parallel or sequentially
	int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)
	
	while (TRUE) { // main shell input loop
		
		// begin parsing code - do not modify
		printf("closh> ");
		fgets(cmd, sizeof(cmd), stdin);
		if (cmd[0] == '\n') continue;
		readCmdTokens(cmd, cmdTokens);
		do {
			printf("  count> ");
			count = readChar() - '0';
		} while (count <= 0 || count > 9);
		
		printf("  [p]arallel or [s]equential> ");
		parallel = (readChar() == 'p') ? TRUE : FALSE;
		do {
			printf("  timeout> ");
			timeout = readChar() - '0';
		} while (timeout < 0 || timeout > 9);
		// end parsing code
		
		
		////////////////////////////////////////////////////////
		//													//
		// TODO: use cmdTokens, count, parallel, and timeout  //
		// to implement the rest of closh					 //
		//													//
		// /////////////////////////////////////////////////////
		
		/* Starter Code
		// just executes the given command once - REPLACE THIS CODE WITH YOUR OWN
		execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
		// doesn't return unless the calling failed
		printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
		exit(1);
		*/
	
	// Loop to create processes until we reach our count
		for(int i = 0; i < count; i++) {
		
		// Fork new process
			int pid = fork();
			if(pid == 0) { // If we are the child process
			
			// Create a process to run our task
				int taskpid = fork();
				if(taskpid == 0) { // If we are the child process
				// Execute given command
					execvp(cmdTokens[0], cmdTokens);
				// Exit with error message if command fails
					printf("Can't execute %s\n", cmdTokens[0]);
					exit(1);
				}
			// Since we always exit inside the conditional, code below is only run
			// by the parent process (i.e., implicitly, else we are the parent)
			
			// Create a timer process for this command
				int timerpid = fork();
				if(timerpid == 0) { // If we are the child process
				// Sleep this thread until we cross our timeout threshold
					sleep(timeout);
				// We have now timed out, so kill task process and exit
					printf("Timeout occured\n");
					kill(taskpid, SIGKILL);
					exit(0);
				}
			// Again, below code is only run by the parent process.
			
			// Wait until task process is terminated. This could be either because
			// it completed or because the timer killed the task process.
				waitpid(taskpid, 0, 0);
			// Kill the timer process (if it is still running) and exit.
				kill(timerpid, SIGKILL);
				exit(0);

			}
		
		// If we are in sequential mode, we must wait for the first iteration to complete
		// before spawning the next iteration. Otherwise, if we are parallel, we simply
		// spawn all our threads as soon as possible.`
			if(!parallel) {
				waitpid(pid, 0, 0);
			}

		}
	
	// This line of code causes our main loop to hold until all the child processs complete.
	// wait(0) blocks execution until any one child process terminates. If there are no child
	// processs which we could wait for, it instead returns -1. Thus, if wait(0) returns -1,
	// we assume all our child processes are finished, and we can continue with our main loop.
		while(wait(0) != -1);

	}
}

