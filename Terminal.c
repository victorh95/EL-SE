#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libTestsPCR/lectureEcriture.h"

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
    char* buffer = NULL;
    buffer = malloc(TAILLEBUF);
    while(strlen(buffer) == 0){
        buffer = litLigne(entree);        
    }
    ecritLigne(sortie, buffer);     
    return 0;
}
