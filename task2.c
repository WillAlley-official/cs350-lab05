#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    char * argv1[] = {"cat", "Makefile", 0};
    char * argv2[] = {"head", "-4", 0};
    char * argv3[] = {"wc", "-l", 0};

    setbuf(stdout, NULL);

	int filedes[2];
	if(pipe(filedes) == -1) return -1;

	// Child 1
	pid_t pid1 = fork();
	if(pid1 == 0) {

		printf("IN CHILD-1 (PID=%d): executing command cat...\n", getpid());
		dup2(filedes[1], STDOUT_FILENO);
		close(filedes[0]);
		close(filedes[1]);
		execvp(argv1[0], &argv1[0]);

	// Child 2
	} else {
	pid_t pid2 = fork();
	if(pid2 == 0) {

		printf("IN CHILD-1 (PID=%d): executing command head...\n", getpid());
		dup2(filedes[0], STDIN_FILENO);
		dup2(filedes[1], STDOUT_FILENO);
		close(filedes[0]);
		close(filedes[1]);
		execvp(argv2[0], &argv2[0]);

	// Child 3
	} else {
	pid_t pid3 = fork();
	if(pid3 == 0) {

		printf("IN CHILD-3 (PID=%d): executing command wc...\n", getpid());
		dup2(filedes[0], STDIN_FILENO);
		close(filedes[1]);
		close(filedes[0]);
		execvp(argv3[0], &argv3[0]);

	// Parent
	} else {

		close(filedes[0]);
		close(filedes[1]);
		wait(NULL);
		printf("IN PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid1);
		wait(NULL);
		printf("IN PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid2);
		wait(NULL);
		printf("IN PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), pid1);

	}}}

    return 0;
}
