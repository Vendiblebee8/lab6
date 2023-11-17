#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t cpid[3] = {0}; 
    int ret = 0;
    int pipe1[2], pipe2[2];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        fprintf(stderr, "Pipe creation failed: %s\n", strerror(errno));
        return 1;
    }

    setbuf(stdout, NULL);

    cpid[1] = fork();
    if (cpid[1] < 0) {
        fprintf(stderr, "fork() 2 failed: %s\n", strerror(errno));
        return 1;
    }
    else if (0 == cpid[1]) {
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        printf("CHILD-2 (PID=%d) is running.\n", getpid());
        write(pipe2[0], "x", 1);
        close(pipe2[0]);
        exit(0);
    }

    cpid[0] = fork();
    if (cpid[0] < 0) {
        fprintf(stderr, "fork() 1 failed: %s\n", strerror(errno));
        return 1;
    }
    else if (0 == cpid[0]) {
        char buf;
        close(pipe1[1]);
        close(pipe2[0]);
        read(pipe2[1], &buf, 1);
        printf("CHILD-1 (PID=%d) is running.\n", getpid());
        write(pipe1[0], "x", 1);
        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);
    }
    
    cpid[2] = fork();
    if (cpid[2] < 0) {
        fprintf(stderr, "fork() 3 failed: %s\n", strerror(errno));
        return 1;
    }
    else if (0 == cpid[2]) {
        char buf;
        close(pipe1[0]);
        read(pipe1[1], &buf, 1);
        printf("CHILD-3 (PID=%d) is running.\n", getpid());
        close(pipe1[1]);
        exit(0);     
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    while ((ret = wait(NULL)) > 0) {
        printf("In PARENT (PID=%d): successfully reaped child (PID=%d)\n", getpid(), ret);
    }

    return 0;
}
