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
    char* buffer = NULL;
    buffer = malloc(TAILLEBUF);
    for(int i = 1; i<3*nombreTests; i++){
        buffer = litLigne(fd); 
        if(i == aleaTest) break;
    }      
    buffer[strcspn(buffer, "\n")] = 0;
    int aleaValidite = alea(24, 168);
    char* validite = NULL;
    validite = malloc(5);
    sprintf(validite, "%d", aleaValidite);
    buffer = message(buffer, "Demande", validite);
    ecritLigne(sortie, buffer);  
    buffer = "";
    while(strlen(buffer) == 0){
        buffer = litLigne(entree);        
    }

    free(buffer);
    close(entree);
    close(sortie);
    close(fd);
    return 0;
}