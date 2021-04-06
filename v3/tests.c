#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "../libTestsPCR/lectureEcriture.h"

struct donnees {
    int entree;
    int sortie;
    char* demande;
    char* reponse;
};

struct ES {
    int entree;
    int sortie;
};

void *threadTerminal(void *terminalES)
{
    struct ES *memoireTerminalES = terminalES;
    char* buffer = malloc(TAILLEBUF);
    buffer = litLigne(memoireTerminalES->entree);
    printf("%s", buffer); 
}

int main(int argc, char *argv[]) {
    long nbTerminaux = 5;
    pthread_t thTerminal[nbTerminaux];
    for(int i = 0; i<nbTerminaux; i++){
        int pid, tube[2];
        pipe(tube);
        pid = fork();
        if(pid == 0){
            char* sortie = malloc(100);
            sprintf(sortie, "%d", tube[1]);  
            execlp("/usr/bin/xterm", "xterm", "-hold", "-e", "./Terminal", "0", sortie, "Numeros_tests_PCR.txt", NULL);
        } else if(pid > 0){
            struct ES *terminalES = malloc(100);
            terminalES->entree = tube[0];
            pthread_create(&thTerminal[i], NULL, threadTerminal, terminalES);      
        }
    }

    for(int i = 0; i<nbTerminaux; i++){
        pthread_join(thTerminal[i],NULL);
    }

    return 0;
}