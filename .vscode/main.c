#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>

#define NB_FILS 4

sem_t semaphore;

void traiter_signal(int sig) {
    printf("Le processus reçoit le signal %d\n", sig);
}

void tache_fils(int id) {
    printf("Le processus fils %d commence sa tâche\n", id);
    sleep(2); // Simulation de tâche complexe
    printf("Le processus fils %d termine sa tâche\n", id);
    kill(getppid(), SIGUSR2); // Envoi d'un signal de confirmation au processus père
}

int main() {
    pid_t pid;
    int i;

    // Création du sémaphore
    sem_init(&semaphore, 0, 0);

    // Installation des gestionnaires de signaux
    signal(SIGUSR1, traiter_signal);
    signal(SIGUSR2, traiter_signal);
    signal(SIGTERM, traiter_signal);

    // Création des processus fils
    for (i = 0; i < NB_FILS; i++) {
        pid = fork();
        if (pid == 0) {
            // Code exécuté par les processus fils
            sem_wait(&semaphore); // Attente de la synchronisation
            tache_fils(i);
            exit(0);
        }
    }

    // Code exécuté par le processus père
    for (i = 0; i < NB_FILS; i++) {
        kill(pid, SIGUSR1); // Envoi d'un signal aux processus fils
    }

    // Attente de la confirmation de chaque processus fils
    for (i = 0; i < NB_FILS; i++) {
        wait(NULL);
        printf("Le processus père reçoit la confirmation du processus fils %d\n", i);
    }

    // Libération du sémaphore
}