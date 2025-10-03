#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void) {
    char * argv1[] = {"cat", "Makefile", 0};
    char * argv2[] = {"head", "-4", 0};
    //char * argv2[] = {"wc", "-l", 0};
    
    setbuf(stdout, NULL);

	pid_t pid1 = fork();
	int filedes[2];
	if(pipe(filedes) == -1) return -1;

	// Child 1
	if(pid1 == 0) {

		printf("IN CHILD-1 (PID=%d): executing command cat...\n", getpid());
		if(close(filedes[0]) == -1) return -1;
		dup2(filedes[1], STDOUT_FILENO);
		execvp(argv1[0], &argv1[0]);

	// Child 2
	} else {
	pid_t pid2 = fork();
	if(pid2 == 0) {

		printf("IN CHILD-2 (PID=%d): executing command head...\n", getpid());
		if(close(filedes[1]) == -1) return -1;
		dup2(filedes[0], STDIN_FILENO);
		execvp(argv2[0], &argv2[0]);

	// Parent
	} else {

		if(close(filedes[0]) == -1) return -1;
		if(close(filedes[1]) == -1) return -1;
		wait(NULL);
		printf("IN PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid1);
		wait(NULL);
		printf("IN PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid2);

	}}

    return 0;
}
