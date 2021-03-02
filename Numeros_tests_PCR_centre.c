#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "libTestsPCR/lectureEcriture.h"
#include "libTestsPCR/alea.h"

void numerosTestsPCRcentre(char* centre, int fd, long nombreTests){
    char* nomFichier = NULL;
    nomFichier = malloc(100);
    strcpy(nomFichier, "Numeros_tests_PCR_");
    strcat(nomFichier, centre);
    strcat(nomFichier, ".txt");
    int fd1 = open(nomFichier, O_CREAT | O_WRONLY, 0644);
    
    char* buffer = NULL;
    buffer = malloc(TAILLEBUF);
    for(int i = 0; i<nombreTests; i++){
        buffer = litLigne(fd);
        buffer[strcspn(buffer, "\n")] = 0;
        int aleaResultat = alea(0, 1);
        if(aleaResultat == 0) strcat(buffer, " Négatif \n");
        else strcat(buffer, " Positif \n");
        ecritLigne(fd1, buffer);
    }
    
    free(nomFichier);
    free(buffer);
}

int main(int argc, char *argv[]) {
    int fd = open("Numeros_tests_PCR.txt", O_RDONLY);
    int nombreTests = strtol(litLigne(fd), NULL, 10);
    //Paris
    numerosTestsPCRcentre("Paris", fd, nombreTests);
    //Nice
    numerosTestsPCRcentre("Nice", fd, nombreTests);
    //Madrid
    numerosTestsPCRcentre("Madrid", fd, nombreTests);
    
    close(fd);
    return 0;
}
