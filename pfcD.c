#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#define PROBS(x) (rand() % (x + 1)); //macro per le probabilità

int msleep(long msec) { //implementazione alta definizione di sleep()
    struct timespec ts;
    int res;
//testaaa
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
void dati(char *a);
void calcoloDati(char ch[], double latit[], double longit[], int o);
double converti(double x);
double distanza(double latA, double lonA, double latB, double lonB);
int lunghezza(char ch[]);
void sigHandler1(int sig);
void sigHandler2(int sig);
void sigHandler3(int sig);

int sigcaugth = 3;
int mod1 = 0;
mod2 = 0;
mod3 = 0;

void sigdisc(int sig) { //prende il pid del processo errato e ne controlla lo stato
    char ch[100];
    char ch2[50];
    FILE *err = fopen("./tmp/err.txt", "r");
    FILE *switchl = fopen("./log/switch.log", "a");
    while (fgets(ch, 100, err))
        ;
    fgets(ch, 100, err);
    int x = strtod(ch, NULL);
    sprintf(ch, "/proc/%i/status", x); //path per i diversi pid
    FILE *stato = fopen(ch, "r");
    for (int i = 0; i < 3; i++)
        fgets(ch2, 100, stato); //scendo direttamente alla terza riga
    fprintf(switchl, "Pid: %d %s", x, ch2);
    fflush(switchl);
    close(stato);
    close(err);
    close(switchl);
}

void sigend(int sig) { //sovrscrittura sigINT
                       //    printf("terminato errore\n");
    sigcaugth = 0;
    signal(SIGINT, sigend);
}

void sigend2(int sig) { //sovrascrittura sigCHLD

    //    printf("terminato con figlio\n");
    sigcaugth--;
}

int main(int argc, char *argv[]) {
    fopen("./log/switch.log", "w");
    int randNum, pfcz, h = 700, pid[6];
    char ch[100];
    srand(time(NULL));
    if (argc < 2) {
        printf("Missing Filename\n");
        return (1);
    }
    // utilizzo i metodi per ottenere i due array con i dati lat e lon
    dati(argv[1]);
    int o = lunghezza(ch);
    double latit[o], longit[o];
    calcoloDati(ch, latit, longit, o);
    for (int i = 0; i < 4; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            if (pid[i] < 0) {
                perror("fork");
                return 1;
            }
            if (i == 0) {
                // comunica con socket
                setpgid(0, 0);
                signal(SIGUSR1, sigHandler1);
                char output[100];
                int serverFd, clientFd, serverLen, clientLen;
                struct sockaddr_un serverUNIXAddress; /*Server address */
                struct sockaddr *serverSockAddrPtr;   /*Ptr to server address*/
                struct sockaddr_un clientUNIXAddress; /*Client address */
                struct sockaddr *clientSockAddrPtr;   /*Ptr to client address*/
                serverSockAddrPtr = (struct sockaddr *)&serverUNIXAddress;
                serverLen = sizeof(serverUNIXAddress);
                clientSockAddrPtr = (struct sockaddr *)&clientUNIXAddress;
                clientLen = sizeof(clientUNIXAddress);
                serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
                serverUNIXAddress.sun_family = AF_UNIX;             /* Set domain type */
                strcpy(serverUNIXAddress.sun_path, "./tmp/socket"); /* Set name */
                unlink("./tmp/socket");                             /* Remove file if it already exists */
                bind(serverFd, serverSockAddrPtr, serverLen);       /*Create file*/
                listen(serverFd, 10);
                clientFd = accept(serverFd, clientSockAddrPtr, &clientLen);
                while (h < o - 1) {
                    if (mod1) {
                        sprintf(output, "%d\n", ((int)round(distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]))) << 2);
                        mod1 = 0;
                    } else {
                        sprintf(output, "%.2f\n", distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]));
                    }
                    write(clientFd, output, strlen(output) + 1);
                    h++;
                    msleep(1000);
                }
                close(clientFd); /* Close the client descriptor */
                fflush(stdout);
                exit(0);
            } else if (i == 1) {
                // comunica con pipe
                setpgid(0, 0);
                signal(SIGUSR1, sigHandler2);
                int fd;
                int messageLen, i;
                char message[100];
                do {                                                                                   /* Keep trying to open the file until successful */
                    fd = open("./tmp/aPipe", O_WRONLY | O_NONBLOCK); /* Open named pipe for writing */ //      if (fd == -1)
                    ;
                    msleep(0.01);
                } while (fd == -1);
                while (h < o - 1) {
                    if (mod2) {
                        sprintf(message, "%d", ((int)round(distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]))) << 2);
                        mod2 = 0;
                    } else {
                        sprintf(message, "%.2f", distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]));
                    }
                    messageLen = strlen(message) + 1;
                    write(fd, message, messageLen);
                    h++;
                    msleep(1000);
                }
                close(fd);
                fflush(stdout);
                exit(0);
            } else if (i == 2) {
                // comunica con file condiviso
                setpgid(0, 0);
                signal(SIGUSR1, sigHandler3);
                while (h < o - 1) {
                    FILE *fpx;
                    char *log3 = "./tmp/speedPFC3.txt";
                    fpx = fopen(log3, "a");
                    char output[100];
                    if (mod3) {
                        sprintf(output, "%d\n", ((int)round(distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]))) << 2);
                        mod3 = 0;
                    } else {
                        sprintf(output, "%.2f\n", distanza(latit[h], longit[h], latit[h + 1], longit[h + 1]));
                    }
                    fputs(output, fpx);
                    fflush(fpx);
                    fclose(fpx);
                    h++;
                    msleep(1000);
                }

                fflush(stdout);
                exit(0);
            } else {
                //generatore fallimenti
                setpgid(0, 0);
                FILE *failures;
                while (1) {
                    msleep(1000);
                    failures = fopen("./log/failures.log", "a");
                    srand(time(NULL));
                    pfcz = rand() % (2 + 1);

                    randNum = PROBS(100);
                    if (randNum == 1) {
                        fprintf(failures, "SIGSTOP per PID %d\n", pid[pfcz]);
                        kill(pid[pfcz], SIGSTOP);
                    }

                    randNum = PROBS(10000)

                        if (randNum == 1) {
                        fprintf(failures, "SIGINT per PID %d\n", pid[pfcz]);
                        kill(pid[pfcz], SIGINT);
                    }

                    randNum = PROBS(10);

                    if (randNum == 1) {
                        fprintf(failures, "SIGCONT per PID %d\n", pid[pfcz]);
                        kill(pid[pfcz], SIGCONT);
                    }

                    randNum = PROBS(10);

                    if (randNum == 1) {
                        fprintf(failures, "SIGUSR1 per PID %d\n", pid[pfcz]);
                        kill(pid[pfcz], SIGUSR1);
                    }
                    fflush(failures);
                    fclose(failures);

                    fflush(stdout);
                    h++;
                }

                exit(0);
            }
        }
    }
    //scrivo i pid dei figli e del processo padre
    signal(SIGUSR2, sigdisc);
    signal(SIGINT, sigend);
    struct sigaction sa;
    sa.sa_handler = sigend2;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        printf("./tmp/errore\n");
    }

    FILE *pids;
    pids = fopen("./tmp/pids.txt", "w");
    for (int i = 0; i < 3; i++) {
        fprintf(pids, "%d\n", pid[i]);
    }
    fprintf(pids, "%d\n", getpid());
    fclose(pids);

    while (sigcaugth) {
        msleep(1000);
    }
    // ottiene i pid di wes e transducers
    pids = fopen("./tmp/pids.txt", "r");
    for (int i = 0; i < 4; i++) {
        fgets(ch, 100, pids);
    }
    for (int i = 5; i < 7; i++) {
        fgets(ch, 100, pids);
        fputs(ch, pids);
        pid[i] = strtod(ch, NULL);
    }
    fclose(pids);
    //uccide tutti
    for (int i = 0; i < 7; i++)
        kill(pid[i], SIGINT);

    removeall();

    return 0;
}
void removeall() {
    remove("./log/speedPFC1.log");
    remove("./log/speedPFC2.log");
    remove("./log/speedPFC3.log");
    remove("./tmp/speedPFC3.txt");
    remove("./tmp/mod1");
    remove("./tmp/mod2");
    remove("./tmp/mod3");
    remove("./log/failures.log");
    remove("./log/status.log");
    remove("./tmp/pids.txt");
    remove("./tmp/useful.txt");
    remove("./tmp/socket");
    remove("./tmp/aPipe");
    remove("./tmp/err.txt");
    remove("./log/switch.log");
}

double converti(double x) { // converte da
    int a;
    double b;
    double c;
    a = x / 100;
    b = (x - a * 100);
    b = b / 60;
    c = a + b;
    return c;
}

//nonostante la funzione calcoli la distanza, essendo i dati aggiornati al
//secondo, rappresenta anche la velocita in m/s
double distanza(double latA, double lonA, double latB, double lonB) {
    /* Definisce le costanti e le variabili */
    const double R = 6377631;
    double lat_alfa, lat_beta, lon_alfa, lon_beta, p, d, fi;
    /* Converte i gradi in radianti */
    lat_alfa = M_PI * latA / 180;
    lat_beta = M_PI * latB / 180;
    lon_alfa = M_PI * lonA / 180;
    lon_beta = M_PI * lonB / 180;
    /* Calcola l'angolo compreso fi */
    fi = fabs(lon_alfa - lon_beta);
    /* Calcola il terzo lato del triangolo sferico */
    p = acos(sin(lat_beta) * sin(lat_alfa) + cos(lat_beta) * cos(lat_alfa) * cos(fi));
    /* Calcola la distanza sulla superficie
                     terrestre R = ~6371 km */
    d = p * R;

    return d;
}

void dati(char *a) { //ricava le righe importanti nel file di input
    FILE *fp;
    FILE *fp2;
    char *g18;
    char *useful = "./tmp/useful.txt";
    char ch[70];
    int x;
    g18 = a;
    fp = fopen(g18, "r");
    fp2 = fopen(useful, "w");
    while (fgets(ch, 100, fp) != NULL) {
        if (ch[3] == 'G' && ch[4] == 'L' && ch[5] == 'L') {
            fputs(ch, fp2);
        }
    }
    fclose(fp);
    fclose(fp2);
}

//calcola 2 vettori contenenti le latitudini e longitudini
void calcoloDati(char ch[], double latit[], double longit[], int o) {
    FILE *fp2;
    char *x;
    int c = 0;
    fp2 = fopen("./tmp/useful.txt", "r");
    fseek(fp2, 0, SEEK_SET);
    while (c < o) {
        fgets(ch, 100, fp2);
        x = ch;
        x = x + 7;
        latit[c] = converti(strtod(x, NULL));
        x = x + 12;
        longit[c] = converti(strtod(x, NULL));
        c++;
    }
    fclose(fp2);
}

int lunghezza(char ch[]) { //calcola il numero di dati
    FILE *fp2;
    int o = 0;
    fp2 = fopen("./tmp/useful.txt", "r");
    while (fgets(ch, 100, fp2)) {
        o++;
    }
    fseek(fp2, 0, SEEK_SET);
    return o;
}
// metodi che permettono sigUSR1 (valore modificato)
void sigHandler1(int sig) {
    mod1 = 1;
}

void sigHandler2(int sig) {
    mod2 = 1;
}

void sigHandler3(int sig) {
    mod3 = 1;
}