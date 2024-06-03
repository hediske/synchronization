#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>

#define NB_FILS 4

sem_t semaphore;
pid_t pids [NB_FILS] ;

void traiter_signal(int sig) {
    printf("Le processus fils reçoit le signal du  père  \n");
}
void traiter_signal_pere(int sig) {
    printf("Le processus père reçoit le signal %d\n", sig);
}

void tache_fils(int id) {
    sem_wait(&semaphore);
    printf("Le processus fils %d commence sa tâche\n", id);
    sleep(3);
    printf("Le processus fils %d termine sa tâche\n", id);
    sem_post(&semaphore);
    kill(getppid(), SIGUSR2);
}

int main() {
    pid_t pid;
    int i;
    int status;
    sem_init(&semaphore, 0, 1);
    signal(SIGUSR1, traiter_signal);
    signal(SIGUSR2, traiter_signal_pere);
    signal(SIGTERM, traiter_signal);

    for (i = 0; i < NB_FILS; i++) {
        pid = fork();
        pids[i] = pid;
        if (pid == 0) {
            tache_fils(i);
            exit(0);
        }
    }

    for (i = 0; i < NB_FILS; i++) {
        kill(pids[i], SIGUSR1); 
    }

   for (i = 1; i <= NB_FILS; i++) {
        wait(&status);
    }

    sem_destroy(&semaphore);

    return 0;

}