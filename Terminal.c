#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libTestsPCR/lectureEcriture.h"
#include "libTestsPCR/alea.h"
#include "libTestsPCR/message.h"

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <descripteur de fichier (entrée)> <descripteur de fichier (sortie)>\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage(argv[0]);

    long entree = strtol(argv[1], NULL, 10);
    long sortie = strtol(argv[2], NULL, 10);
    dup2(entree, 0);
    dup2(sortie, 1);
    
    int fd = open("Numeros_tests_PCR.txt", O_RDONLY);
    int nombreTests = strtol(litLigne(fd), NULL, 10);
    aleainit();
    int aleaTest = alea(1, 3*nombreTests);
    char* buffer = malloc(TAILLEBUF);
    for(int i = 1; i<3*nombreTests; i++){
        buffer = litLigne(fd); 
        if(i == aleaTest) break;
    }      
    buffer[strcspn(buffer, "\n")] = 0;
    int aleaValidite = alea(24, 168);
    char* validite = malloc(5);
    sprintf(validite, "%d", aleaValidite);
    char* nTestEnvoye = malloc(50);
    nTestEnvoye = buffer;
    buffer = message(nTestEnvoye, "Demande", validite);
    ecritLigne(sortie, buffer);  
          
    char* nTestRecu = malloc(50);
    char* type = malloc(50);
    char* valeur = malloc(50);
    if(!decoupe(litLigne(entree), nTestRecu, type, valeur)){
        perror("Erreur de la fonction decoupe");
        exit(-1);
    };
    if(strcmp(type, "Reponse") != 0 || strcmp(nTestRecu, nTestEnvoye) != 0){
        ecritLigne(1, "Mauvais message. \n");
        exit(-1);
    }    
    if(strcmp(valeur, "1") == 0){
        ecritLigne(1, "Demande acceptée. \n");
    } else{
        ecritLigne(1, "Demande refusée. \n");
    }

    free(buffer);
    free(validite);
    free(nTestEnvoye);
    free(nTestRecu);
    free(type);
    free(valeur);
    close(entree);
    close(sortie);
    close(fd);
    return 0;
}