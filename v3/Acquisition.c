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

sem_t semSortie;
sem_t semEcrit;
sem_t semDemandeValidation;
//sem_t semDemandeInterArchives;

void usage(char * basename){
    fprintf(stderr,
        "Utilisation : %s <taille de la mémoire> <nom du centre> <code à 4 chiffres du centre> <nom du fichier des résultats des tests PCR> <nombre terminaux>\n",
        basename);
    exit(1);
}

void *threadTerminalEntree(void *terminalES){
    struct ES *memoireTerminalES = terminalES;
    char* buffer = malloc(TAILLEBUF);
    while(1){         
        buffer = litLigne(memoireTerminalES->entree); 
        for(int i = 0; i < tailleMemoire; i++){           
            if(strcmp(memoire[i].demande, "") == 0){               
                sem_wait(&semEcrit);
                memoire[i].entree = memoireTerminalES->entree;
                memoire[i].sortie = memoireTerminalES->sortie;
                memoire[i].demande = buffer;
                sem_post(&semEcrit);                
            }            
            sem_post(&semDemandeValidation);
            //sem_post(&semDemandeInterArchives);           
        }    
    }
}

void *threadTerminalSortie(void *inutilise){
    while(1){   
        sem_wait(&semSortie);                 
        for(int i = 0; i < tailleMemoire; i++){           
            if(strcmp(memoire[i].reponse, "") != 0){
                ecritLigne(memoire[i].sortie, memoire[i].reponse);
                memoire[i].demande = "";
                memoire[i].reponse = "";
            }                 
        }    
    }
}

void *threadValidation(void *validationES){
    struct ES *memoireValidationES = validationES;
    char* buffer = malloc(TAILLEBUF);
    while(1){     
        sem_wait(&semDemandeValidation);     
        for(int i = 0; i < tailleMemoire; i++){        
            if((strcmp(memoire[i].reponse, "") == 0) && (memoire[i].demande[1] - '0' == centre)){
                ecritLigne(memoireValidationES->sortie, memoire[i].demande);
                buffer = litLigne(memoireValidationES->entree);
                sem_wait(&semEcrit);
                memoire[i].reponse = buffer;
                sem_post(&semEcrit);
                sem_post(&semSortie);
            }             
        }    
    }   
}

/*void *threadInterArchives(void *inutilise){
    char* buffer = malloc(TAILLEBUF);
    while(1){        
        for(int i = 0; i < tailleMemoire; i++){
            sem_wait(&semDemsemDemandeInterArchivesande2);
            if((strcmp(memoire[i].reponse, "") == 0) && (memoire[i].demande[1] - '0' != centre)){
                ecritLigne(fd2, memoire[i].demande);
                buffer = litLigne(fd5);
                if(buffer != NULL) memoire[i].reponse = buffer;
                sem_post(&semLibre);
            }
        }      
    }
}*/

int main(int argc, char *argv[]){
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

    sem_init(&semSortie, 0, 0);
    sem_init(&semDemandeValidation, 0, 0);
    //sem_init(&semDemandeInterArchives, 0, 0);
    sem_init(&semEcrit, 0, 1);

    //Création des terminaux
    long nbTerminaux = strtol(argv[5], NULL, 10);
    pthread_t thTerminalEntree[nbTerminaux];
    for(int i = 0; i<nbTerminaux; i++){
        int pid, fdpipeTerminalEntree[2], fdpipeTerminalSortie[2];
        pipe(fdpipeTerminalEntree), pipe(fdpipeTerminalSortie);

        pid = fork();
        if(pid == 0){
            char* entree = malloc(100);
            sprintf(entree, "%d", fdpipeTerminalEntree[0]); 
            char* sortie = malloc(100);
            sprintf(sortie, "%d", fdpipeTerminalSortie[1]); 
            execlp("/usr/bin/xterm", "xterm", "-hold", "-e", "./Terminal", entree, sortie, "Numeros_tests_PCR.txt", NULL);
        } else if(pid > 0){
            struct ES *terminalES = malloc(100);
            terminalES->entree = fdpipeTerminalSortie[0];
            terminalES->sortie = fdpipeTerminalEntree[1];
            pthread_create(&thTerminalEntree[i], NULL, threadTerminalEntree, terminalES);
        }
    }
    pthread_t thTerminalSortie;
    pthread_create(&thTerminalSortie, NULL, threadTerminalSortie, NULL);

    //Création du serveur de validation
    pthread_t thValidation;
    int pid, fdpipeValidationEntree[2], fdpipeValidationSortie[2];
    pipe(fdpipeValidationEntree), pipe(fdpipeValidationSortie);
    
    pid = fork();
    if(pid == 0){
        char* entree = malloc(100);
        sprintf(entree, "%d", fdpipeValidationEntree[0]); 
        char* sortie = malloc(100);
        sprintf(sortie, "%d", fdpipeValidationSortie[1]); 
        execlp("./Validation", "./Validation", entree, sortie, argv[4], NULL);
    } else if(pid > 0){
        struct ES *validationES = malloc(100);
        validationES->entree = fdpipeValidationSortie[0];
        validationES->sortie = fdpipeValidationEntree[1];
        pthread_create(&thValidation, NULL, threadValidation, validationES);
    }
    
    //Création du serveur de interArchives
    //pthread_t thInterArchives;
    //pthread_create(&thInterArchives, NULL, threadInterArchives, NULL);
    //pthread_join(thInterArchives,NULL);

    for(int i = 0; i<nbTerminaux; i++){
        pthread_join(thTerminalEntree[i],NULL);
    }
    pthread_join(thValidation,NULL);

    return 0;
}