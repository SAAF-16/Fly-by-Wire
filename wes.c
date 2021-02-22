#include <errno.h>
#include <signal.h>;
#include <stdio.h>;
#include <stdlib.h>;
#include <time.h>
#include <unistd.h>;

int sigcaught = 1;
void sighandler(int sig) { //handler per gestire SIGINT
    sigcaught = 0;
}
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
int main(int argc, char *argv[]) {
    FILE *log1;
    FILE *log2;
    FILE *log3;
    FILE *status;
    FILE *pids;
    FILE *err;
    int z = 1;
    char strp[6], str1[6], str2[6], str3[6];
    int index1 = 0, index2 = 0, index3 = 0;
    int i1 = 0, i2 = 0, i3 = 0;
    double match[] = {-1.00, -1.00, -1.00};
    int ind1 = 0, ind2 = 0, ind3 = 0;
    int olind1 = 0, olind2 = 0, olind3 = 0;
    int ipids[4];
    char c = 0;
    int x = 1;

    while (1 && sigcaught) {
        if (fopen("./tmp/pids.txt", "r") && x) {
            pids = fopen("./tmp/pids.txt", "r");
            for (int i = 0; i < 4; i++) {
                fgets(strp, 100, pids);
                ipids[i] = strtod(strp, NULL);
            }
            fclose(pids);
            pids = fopen("./tmp/pids.txt", "a+");
            fprintf(pids, "%d", getpid());
            x = 0;
            fclose(pids);
        }
        ind1 = 0, ind2 = 0, ind3 = 0;
        if (fopen("./log/speedPFC1.log", "r") && fopen("./log/speedPFC2.log", "r") && fopen("./log/speedPFC3.log", "r")) {
            log1 = fopen("./log/speedPFC1.log", "r");
            log2 = fopen("./log/speedPFC2.log", "r");
            log3 = fopen("./log/speedPFC3.log", "r");
            /////////////////SOCKET///////////////////////////////////////////
            while (fgets(str1, 100, log1))
                ind1++;
            if (olind1 == ind1) {
                match[0] = -1;
            } else {
                match[0] = strtod(str1, NULL);
                olind1 = ind1;
            }
            /////////////////PIPE/////////////////////////////////////////////
            while (fgets(str2, 100, log2))
                ind2++;
            if (olind2 == ind2) {
                match[1] = -2;
            } else {
                match[1] = strtod(str2, NULL);
                olind2 = ind2;
            }
            ///////////////FILE CONDIVISO//////////////////////////////////////
            while (fgets(str3, 100, log3))
                ind3++;
            if (olind3 == ind3) {
                match[2] = -3;
            } else {
                match[2] = strtod(str3, NULL);
                olind3 = ind3;
            }
            ///////////////////////////////////////////////////////////////////
            status = fopen("./log/status.log", "a+");
            err = fopen("./tmp/err.txt", "a");
            if (match[0] == match[1] && match[1] == match[2]) {
                fprintf(status, "%.2f %.2f %.2f OK!\n", match[0], match[1], match[2]);
                printf("%.2f %.2f %.2f OK!\n", match[0], match[1], match[2]);
            } else if (match[0] == match[1] || match[1] == match[2] || match[2] == match[0]) {
                if (match[0] == match[1]) {
                    printf("%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[2]);
                    fprintf(status, "%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[2]);
                    fprintf(err, "%d\n", ipids[2]);
                    kill(ipids[3], SIGUSR2);
                } else if (match[1] == match[2]) {
                    printf("%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[0]);
                    fprintf(status, "%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[0]);
                    fprintf(err, "%d\n", ipids[0]);
                    kill(ipids[3], SIGUSR2);
                } else if (match[2] == match[0]) {
                    printf("%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[1]);
                    fprintf(status, "%.2f %.2f %.2f processo con pid %d non congruo\n", match[0], match[1], match[2], ipids[1]);
                    fprintf(err, "%d\n", ipids[1]);
                    kill(ipids[3], SIGUSR2);
                }
                fflush(stdout);
                fclose(err);
            } else if (match[0] != match[1] && match[1] != match[2]) {
                printf("%.2f %.2f %.2f EMERGENZA!\n", match[0], match[1], match[2]);
                fprintf(status, "%.2f %.2f %.2f EMERGENZA!\n", match[0], match[1], match[2]);
                fflush(NULL);
                fflush(status);
                z = 0;
                printf("Emergenza sto terminando...\n");
                msleep(1000);
                kill(ipids[3], SIGINT);
            }
            fclose(log1);
            fclose(log2);
            fclose(log3);
            fclose(status);
            fflush(NULL);
            msleep(1000);
        } else {
            msleep(500);
        }
    }
}