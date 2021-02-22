#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int pid[3];
    if (argc < 2) {
        printf("Missing Filename\n");
        return (1);
    }
    for (int i = 0; i < 3; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            perror("fork");
            return 1;
        }
        if (pid[i] == 0) {
            if (i == 2) {
                argv[0] = "./bin/tra";
                argv[1] = NULL;
                execv(argv[0], argv);
            } else if (i == 1) {
                argv[0] = "./bin/wes";
                argv[1] = NULL;
                execv(argv[0], argv);
            } else if (i == 0) {
                argv[0] = "./bin/pfc";
                execv(argv[0], argv);
            }
        }
    }
for(int i=0;i<3;i++)
    wait(NULL);
}