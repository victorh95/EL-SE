#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libTestsPCR/lectureEcriture.h"
#include "libTestsPCR/alea.h"

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <descripteur de fichier (entrée)> <descripteur de fichier (sortie)> <nombre de tests PCR générés par centre>\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) usage(argv[0]);

    long entree = strtol(argv[1], NULL, 10);
    long sortie = strtol(argv[2], NULL, 10);
    dup2(entree, 0);
    dup2(sortie, 1);
    
    int fd = open("Numeros_tests_PCR.txt", O_RDONLY);
    long nombresTests = strtol(argv[3], NULL, 10);
    aleainit();
    int aleatests = alea(1, 3*nombresTests);
    char* buffer = NULL;
    buffer = malloc(TAILLEBUF);
    for(int i = 1; i<3*nombresTests; i++){
        buffer = litLigne(fd); 
        if(i == aleatests) break;
    }      
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