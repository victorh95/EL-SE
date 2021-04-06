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

//sem_t semLibre;
sem_t semEcrit;
//sem_t semDemande1;
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
    fcntl(memoireTerminalES->entree, F_SETFL, fcntl(memoireTerminalES->entree, F_GETFL) | O_NONBLOCK);
    while(1){                   
        for(int i = 0; i < tailleMemoire; i++){    
            //sem_wait(&semLibre);        
            if((strcmp(memoire[i].reponse, "") != 0) && (memoire[i].sortie == memoireTerminalES->sortie)){
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
            //sem_post(&semDemande1);
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
            //sem_wait(&semDemande1);        
            if((strcmp(memoire[i].reponse, "") == 0) && (memoire[i].demande[1] - '0' == centre)){
                ecritLigne(memoireValidationES->sortie, memoire[i].demande);
                buffer = litLigne(memoireValidationES->entree);
                if(buffer != NULL){
                    sem_wait(&semEcrit);
                    memoire[i].reponse = buffer;
                    sem_post(&semEcrit);
                }  
                //sem_post(&semLibre);
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
            if((strcmp(memoire[i].reponse, "") == 0) && (memoire[i].demande[1] - '0' != centre)){
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

    tailleMemoire = strtol(argv[1], NULL, 10);
    memoire = calloc(tailleMemoire, TAILLEBUF);

    for(int i = 0; i < tailleMemoire; i++){
        memoire[i].demande = "";
        memoire[i].reponse = "";
    }

    if(strcmp(argv[2], "Paris") == 0) centre = 0;
    else if(strcmp(argv[2], "Nice") == 0) centre = 1;
    else centre = 2;

    //sem_init(&semLibre, 0, tailleMemoire);
    //sem_init(&semDemande1, 0, 0);
    //sem_init(&semDemande2, 0, 0);
    sem_init(&semEcrit, 0, 1);
    //pthread_t thInterArchives;
    //pthread_create(&thInterArchives, NULL, threadInterArchives, NULL);
    //pthread_join(thInterArchives,NULL);

    //Création des terminaux
    long nbTerminaux = strtol(argv[5], NULL, 10);
    pthread_t thTerminal[nbTerminaux];
    for(int i = 0; i<nbTerminaux; i++){
        int pid, fdpipeTerminalEntree[2], fdpipeTerminalSortie[2];
        pipe(fdpipeTerminalEntree), pipe(fdpipeTerminalSortie);

        pid = fork();
        if(pid == 0){
            char* entree = malloc(100);
            sprintf(entree, "%d", fdpipeTerminalEntree[0]); 
            char* sortie = malloc(100);
            sprintf(sortie, "%d", fdpipeTerminalSortie[1]); 
            execlp("/usr/bin/xterm", "xterm", "-hold", "-e", "./Terminal", entree, sortie, argv[4], NULL);
        } else if(pid > 0){
            struct ES *terminalES = malloc(100);
            terminalES->entree = fdpipeTerminalSortie[0];
            terminalES->sortie = fdpipeTerminalEntree[1];
            pthread_create(&thTerminal[i], NULL, threadTerminal, terminalES);
        }
    }

    //Création du serveur de validation
    pthread_t thValidation;
    int pid, fdpipeValidationEntree[2], fdpipeValidationSortie[2];
    pipe(fdpipeValidationEntree), pipe(fdpipeValidationSortie);
    
    char* nomFichier = malloc(100); 
    strcpy(nomFichier, "Numeros_tests_PCR_");
    strcat(nomFichier, argv[2]);
    strcat(nomFichier, ".txt");

    pid = fork();
    if(pid == 0){
        char* entree = malloc(100);
        sprintf(entree, "%d", fdpipeValidationEntree[0]); 
        char* sortie = malloc(100);
        sprintf(sortie, "%d", fdpipeValidationSortie[1]); 
        execlp("./Validation", "./Validation", entree, sortie, nomFichier, NULL);
    } else if(pid > 0){
        struct ES *validationES = malloc(100);
        validationES->entree = fdpipeValidationSortie[0];
        validationES->sortie = fdpipeValidationEntree[1];
        pthread_create(&thValidation, NULL, threadValidation, validationES);
    }
    
    for(int i = 0; i<nbTerminaux; i++){
        pthread_join(thTerminal[i],NULL);
    }
    pthread_join(thValidation,NULL);

    return 0;
}