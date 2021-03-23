#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../libTestsPCR/lectureEcriture.h"

struct donnees {
    int entree;
    int sortie;
    char* demande;
    char* reponse;
};

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <taille de la mémoire (nombre de demandes de validation)> <nom du centre>\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage(argv[0]);

    long tailleMemoire = strtol(argv[1], NULL, 10);
    struct donnees* memoire = calloc(tailleMemoire, TAILLEBUF);; 
    char* buffer = malloc(TAILLEBUF);

    int fd0 = open("Acquisition_Terminal.txt", O_WRONLY);
    int fd1 = open("Acquisition_Validation.txt", O_WRONLY);
    int fd2 = open("Acquisition_InterArchives.txt", O_WRONLY);
    int fd3 = open("Terminal_Acquisition.txt", O_RDONLY);
    int fd4 = open("Validation_Acquisition.txt", O_RDONLY);
    int fd5 = open("InterArchives_Acquisition.txt", O_RDONLY);
    
    for(int i = 0; i < tailleMemoire; i++){
        memoire[i].entree = fd3; 
        memoire[i].sortie = fd0;
        memoire[i].demande = "";
        memoire[i].reponse = "";
    } 

    int centre;
    if(strcmp(argv[2], "Paris") == 0) centre = 0;
    else if(strcmp(argv[2], "Nice") == 0) centre = 1;
    else centre = 2;
    
    int stop = 0;

    while(1){
        for(int i = 0; i < tailleMemoire; i++){
            if((strcmp(memoire[i].reponse, "") != 0)){
                ecritLigne(memoire[i].sortie, memoire[i].reponse);
                memoire[i].demande = "";
                memoire[i].reponse = "";
            }
            buffer = litLigne(memoire[i].entree);
            if(i == 0 && buffer == NULL) stop = 1;
            if(buffer != NULL) memoire[i].demande = buffer;
        }
        if(stop == 1) return 0;

        for(int i = 0; i < tailleMemoire; i++){
            if(memoire[i].demande[1] - '0' == centre){
                ecritLigne(fd1, memoire[i].demande);
                buffer = litLigne(fd4);
                if(buffer != NULL) memoire[i].reponse = buffer;
            } 
        }     

        for(int i = 0; i < tailleMemoire; i++){
            if(memoire[i].demande[1] - '0' != centre){
                ecritLigne(fd2, memoire[i].demande);
                buffer = litLigne(fd5);
                if(buffer != NULL) memoire[i].reponse = buffer;
            }
        }
    }
    
    return 0;
}