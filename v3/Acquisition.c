#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

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

long tailleMemoire;
struct donnees* memoire;
int centre;

sem_t semLibre;
sem_t semEcrit;
sem_t semDemande1;
//sem_t semDemande2;

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <taille de la mémoire> <nom du centre> <code à 4 chiffres du centre> <nom du fichier des résultats des tests PCR> <nombre terminaux>\n",
        basename);
    exit(1);
}

void *threadTerminal(void *terminalES)
{
    struct ES *memoireTerminalES = terminalES;
    char* buffer = malloc(TAILLEBUF);
    while(1){                     
        for(int i = 0; i < tailleMemoire; i++){    
            sem_wait(&semLibre);        
            if(strcmp(memoire[i].reponse, "") != 0){
                ecritLigne(memoire[i].sortie, memoire[i].reponse);
                memoire[i].demande = "";
                memoire[i].reponse = "";
            }
            if(strcmp(memoire[i].demande, "") == 0){
                buffer = litLigne(memoireTerminalES->entree);
                if(buffer != NULL){
                    sem_wait(&semEcrit);
                    memoire[i].entree = memoireTerminalES->entree;
                    memoire[i].sortie = memoireTerminalES->sortie;
                    memoire[i].demande = buffer;
                    sem_post(&semEcrit);
                }
            }            
            sem_post(&semDemande1);
            //sem_post(&semDemande2);           
        }       
    }
}

void *threadValidation(void *validationES)
{
    struct ES *memoireValidationES = validationES;
    char* buffer = malloc(TAILLEBUF);
    while(1){        
        for(int i = 0; i < tailleMemoire; i++){   
            sem_wait(&semDemande1);         
            if(memoire[i].demande[1] - '0' == centre){
                ecritLigne(memoireValidationES->sortie, memoire[i].demande);
                buffer = litLigne(memoireValidationES->entree);
                if(buffer != NULL) memoire[i].reponse = buffer; 
                sem_post(&semLibre);
            }             
        }    
    }   
}

/*void *threadInterArchives(void *inutilise)
{
    char* buffer = malloc(TAILLEBUF);
    while(1){        
        for(int i = 0; i < tailleMemoire; i++){
            sem_wait(&semDemande2);
            if(memoire[i].demande[1] - '0' != centre){
                ecritLigne(fd2, memoire[i].demande);
                buffer = litLigne(fd5);
                if(buffer != NULL) memoire[i].reponse = buffer;
                sem_post(&semLibre);
            }
        }      
    }
}*/

int main(int argc, char *argv[]) {
    if (argc != 6) usage(argv[0]);

    //Création des terminaux
    int pid; 
    long nbTerminaux = strtol(argv[5], NULL, 10);
    pthread_t thTerminal[nbTerminaux];
    int fdpipeTerminalEntree[2];
    int fdpipeTerminalSortie[2];
    for(int i = 0; i<nbTerminaux; i++){
        if ((pipe(fdpipeTerminalEntree) == -1) || (pipe(fdpipeTerminalSortie) == -1)){
            perror("Erreur lors de la création du ou des pipe");
            exit(-1);
        }

        switch(pid = fork()){
        case -1:
            perror("Erreur de fork");
            exit(-1);
        case 0:
            dup2(fdpipeTerminalSortie[0], 0);
            close(fdpipeTerminalSortie[0]);
            close(fdpipeTerminalEntree[1]);
            execlp("xterm -e Terminal", "xterm -e Terminal", fdpipeTerminalEntree[0], fdpipeTerminalSortie[1], argv[1], NULL);
        default :
            dup2(fdpipeTerminalEntree[1], 1);
            close(fdpipeTerminalSortie[0]);
            close(fdpipeTerminalEntree[1]);
            struct ES *terminalES = malloc(100);
            terminalES->entree = fdpipeTerminalSortie[0];
            terminalES->sortie = fdpipeTerminalEntree[1];
            pthread_create(&thTerminal[i], NULL, threadTerminal, terminalES);
            pthread_join(thTerminal[i],NULL);
        }
    }

    //Création du serveur de validation
    pthread_t thValidation;
    int fdpipeValidationEntree[2];
    int fdpipeValidationSortie[2];
    if ((pipe(fdpipeValidationEntree) == -1) || (pipe(fdpipeValidationSortie) == -1)){
            perror("Erreur lors de la création du ou des pipe");
            exit(-1);
        }

    switch(pid = fork()){
    case -1:
        perror("Erreur de fork");
        exit(-1);
    case 0:
        dup2(fdpipeValidationSortie[0], 0);
        close(fdpipeValidationSortie[0]);
        close(fdpipeValidationEntree[1]);
        execlp("./Validation", "./Validation", fdpipeValidationEntree[0], fdpipeValidationSortie[1], argv[4], NULL);
    default :
        dup2(fdpipeValidationEntree[1], 1);
        close(fdpipeValidationSortie[0]);
        close(fdpipeValidationEntree[1]);
        struct ES *validationES = malloc(100);
        validationES->entree = fdpipeValidationSortie[0];
        validationES->sortie = fdpipeValidationEntree[1];
        pthread_create(&thValidation, NULL, threadValidation, validationES);
        pthread_join(thValidation,NULL);
    }

    tailleMemoire = strtol(argv[1], NULL, 10);
    memoire = calloc(tailleMemoire, TAILLEBUF);

    for(int i = 0; i < tailleMemoire; i++){
        memoire[i].demande = "";
        memoire[i].reponse = "";
    }

    if(strcmp(argv[2], "Paris") == 0) centre = 0;
    else if(strcmp(argv[2], "Nice") == 0) centre = 1;
    else centre = 2;

    sem_init(&semLibre, 0, tailleMemoire);
    sem_init(&semDemande1, 0, 0);
    //sem_init(&semDemande2, 0, 0);
    sem_init(&semEcrit, 0, 1);
    //pthread_t thInterArchives;
    //pthread_create(&thInterArchives, NULL, threadInterArchives, NULL);
    //pthread_join(thInterArchives,NULL);
    
    return 0;
}