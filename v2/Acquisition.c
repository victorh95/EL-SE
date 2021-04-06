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

long tailleMemoire;
struct donnees* memoire;
int fd0;
int fd1;
int fd2;
int fd3;
int fd4;
int fd5;
int centre;
int stop;

sem_t semLibre;
sem_t semDemande1;
sem_t semDemande2;

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <taille de la mémoire (nombre de demandes de validation)> <nom du centre>\n",
        basename);
    exit(1);
}

void *threadTerminal(void *inutilise)
{
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
                buffer = litLigne(memoire[i].entree);
                if(buffer == NULL) stop++;
                if(buffer != NULL){
                    memoire[i].demande = buffer;
                }  
            }            
            sem_post(&semDemande1);
            sem_post(&semDemande2);           
        }       
        if(stop == tailleMemoire) pthread_exit(NULL);
        else stop = 0;
    }
}

void *threadValidation(void *inutilise)
{
    char* buffer = malloc(TAILLEBUF);
    while(1){        
        for(int i = 0; i < tailleMemoire; i++){   
            sem_wait(&semDemande1);         
            if((strcmp(memoire[i].reponse, "") == 0) && (memoire[i].demande[1] - '0' == centre)){
                ecritLigne(fd1, memoire[i].demande);
                buffer = litLigne(fd4);
                if(buffer != NULL) memoire[i].reponse = buffer; 
                sem_post(&semLibre);
            }             
        }    
        if(stop == tailleMemoire) pthread_exit(NULL);  
    }   
}

void *threadInterArchives(void *inutilise)
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
        if(stop == tailleMemoire) pthread_exit(NULL);      
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage(argv[0]);

    tailleMemoire = strtol(argv[1], NULL, 10);
    memoire = calloc(tailleMemoire, TAILLEBUF);

    fd0 = open("Acquisition_Terminal.txt", O_WRONLY);
    fd1 = open("Acquisition_Validation.txt", O_WRONLY);
    fd2 = open("Acquisition_InterArchives.txt", O_WRONLY);
    fd3 = open("Terminal_Acquisition.txt", O_RDONLY);
    fd4 = open("Validation_Acquisition.txt", O_RDONLY);
    fd5 = open("InterArchives_Acquisition.txt", O_RDONLY);

    for(int i = 0; i < tailleMemoire; i++){
        memoire[i].entree = fd3; 
        memoire[i].sortie = fd0;
        memoire[i].demande = "";
        memoire[i].reponse = "";
    }

    if(strcmp(argv[2], "Paris") == 0) centre = 0;
    else if(strcmp(argv[2], "Nice") == 0) centre = 1;
    else centre = 2;

    stop = 0;

    sem_init(&semLibre, 0, tailleMemoire);
    sem_init(&semDemande1, 0, 0);
    sem_init(&semDemande2, 0, 0);
    pthread_t thTerminal, thValidation, thInterArchives;
    pthread_create(&thTerminal, NULL, threadTerminal, NULL);
    pthread_create(&thValidation, NULL, threadValidation, NULL);
    pthread_create(&thInterArchives, NULL, threadInterArchives, NULL);
    pthread_join(thTerminal,NULL);
    pthread_join(thValidation,NULL);
    pthread_join(thInterArchives,NULL);
    
    return 0;
}