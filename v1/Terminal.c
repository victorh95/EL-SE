#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../libTestsPCR/lectureEcriture.h"
#include "../libTestsPCR/alea.h"
#include "../libTestsPCR/message.h"

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <descripteur de fichier (entrée)> <descripteur de fichier (sortie)> <nom du fichier des tests PCR>\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) usage(argv[0]);

    long entree = strtol(argv[1], NULL, 10);
    long sortie = strtol(argv[2], NULL, 10);

    int fd = open(argv[3], O_RDONLY);
    int nombreTests = strtol(litLigne(fd), NULL, 10);
    aleainit();
    int aleaTest = alea(1, nombreTests);
    char* buffer = malloc(TAILLEBUF);
    for(int i = 1; i < nombreTests; i++){
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
    printf("%s", buffer);  

    char* nTestRecu = malloc(50);
    char* type = malloc(50);
    char* valeur = malloc(50);
    buffer = litLigne(entree);
    printf("%s", buffer);
    if(!decoupe(buffer, nTestRecu, type, valeur)){
        perror("Erreur de la fonction decoupe");
        exit(-1);
    }
    if(strcmp(type, "Reponse") != 0 || strcmp(nTestRecu, nTestEnvoye) != 0){
        printf("Mauvais message. \n");
        exit(-1);
    }    
    if(strcmp(valeur, "1") == 0){
        printf("Demande acceptée. \n");
    } else{
        printf("Demande refusée. \n");
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