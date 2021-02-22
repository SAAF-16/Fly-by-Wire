#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h> /* For S_IFIFO */
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int readLine(int fd, char *str) {
    /* Read a single '\0'-terminated line into str from fd */
    /* Return 0 when the end-of-input is reached and 1 otherwise */
    int n;
    do {                      /* Read characters until '\0' or end-of-input */
        n = read(fd, str, 1); /* Read one character */
    } while (n > 0 && *str++ != '\0');
    return (n > 0); /* Return false if end-of-input */
}

int sigcaught = 1;
void sighandler(int sig) { //handler per gestire SIGINT
    sigcaught = 0;
}
int main(void) {
    signal(SIGINT, sighandler);
    FILE *pids;
    FILE *fp4;
    FILE *fp5;
    FILE *fp6;
    FILE *fp7;
    int fd;
    char str1[6];
    char str2[6];
    char str3[6];
    char *log1 = "./log/speedPFC1.log";
    char *log2 = "./log/speedPFC2.log";
    char *log3 = "./log/speedPFC3.log";
    char *log3t = "./tmp/speedPFC3.txt";
    int i = 0, index = -1;
    int clientFd, serverLen, result;
    struct sockaddr_un serverUNIXAddress;
    struct sockaddr *serverSockAddrPtr;
    serverSockAddrPtr = (struct sockaddr *)&serverUNIXAddress;
    serverLen = sizeof(serverUNIXAddress);
    clientFd = socket(AF_UNIX, SOCK_STREAM | O_NONBLOCK, 0);
    serverUNIXAddress.sun_family = AF_UNIX;             /* Server domain */
    strcpy(serverUNIXAddress.sun_path, "./tmp/socket"); /*Server name*/
    unlink("./tmp/aPipe");
    int x = 1;
    /* Create named pipe */
    mkfifo("./tmp/aPipe", S_IFIFO);
    /* Change its permissions */
    chmod("./tmp/aPipe", 0660);
    fd = open("./tmp/aPipe", O_RDONLY | O_NONBLOCK); /* Open it for reading */

    while (1 && sigcaught) {
        /////////////////////////////////////////////////////////////
        if (x && fopen("./tmp/pids.txt", "r")) {
            pids = fopen("./tmp/pids.txt", "a");
            fprintf(pids, "%d\n", getpid());
            x = 0;
            fclose(pids);
        }
        ///////////////  FILE CONDIVISO   ////////////////////////////
        if ((fp4 = fopen(log3t, "r"))) {
            fp5 = fopen(log3, "a");
            i = 0;
            while (fgets(str3, 100, fp4)) {
                i++;
                if (i > index) {
                    fputs(str3, fp5);
                    index = i;
                }
                //i++;
            }
            fclose(fp5);
            fclose(fp4);
        }
        //////////////   PIPE   ////////////////////////////////
        fp6 = fopen(log2, "a");
        while (readLine(fd, str2)) {
            fputs(str2, fp6);
            fputs("\n", fp6);
        }
        fflush(fp6);
        fclose(fp6);
        /////////////////   SOCKET   //////////////////////////
        connect(clientFd, serverSockAddrPtr, serverLen);
        fp7 = fopen(log1, "a");
        while (readLine(clientFd, str1)) {
            fputs(str1, fp7);
        }
        fclose(fp7);

        //////////////////////////////////////////////////////////
        fflush(stdout);
        fflush(stdin);
        msleep(50);
    }
    close(clientFd);
    close(fd);
    unlink("./tmp/aPipe");
    unlink("./tmp/socket");
}
