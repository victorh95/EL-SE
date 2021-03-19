#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "../libTestsPCR/lectureEcriture.h"
#include "../libTestsPCR/alea.h"

void numerosTestsPCRcentre(char* centre, int fd, char* nombreTests){
    char* nomFichier = malloc(100); 
    strcpy(nomFichier, "Numeros_tests_PCR_");
    strcat(nomFichier, centre);
    strcat(nomFichier, ".txt");
    int fd1 = open(nomFichier, O_CREAT | O_WRONLY, 0644);
    ecritLigne(fd1, nombreTests);

    long nombreTests_long = strtol(nombreTests, NULL, 10);
    char* buffer = malloc(TAILLEBUF);
    int date = time(NULL); 
    int aleaDate;
    char* testDate = malloc(50);
    int aleaResultat;
    for(int i = 0; i<nombreTests_long; i++){
        buffer = litLigne(fd);
        buffer[strcspn(buffer, "\n")] = 0;
        aleaDate = alea(0, 7 * 86400);
        sprintf(testDate, " %d", date - aleaDate);
        strcat(buffer, testDate);
        aleaResultat = alea(0, 1);
        if(aleaResultat == 0) strcat(buffer, " Négatif \n");
        else strcat(buffer, " Positif \n");
        ecritLigne(fd1, buffer);
    }
    
    free(nomFichier);
    free(buffer);
    free(testDate);
    close(fd1);
}

int main(int argc, char *argv[]) {
    int fd = open("Numeros_tests_PCR.txt", O_RDONLY);
    char* nombreTests = litLigne(fd);
    //Paris
    numerosTestsPCRcentre("Paris", fd, nombreTests);
    //Nice
    numerosTestsPCRcentre("Nice", fd, nombreTests);
    //Madrid
    numerosTestsPCRcentre("Madrid", fd, nombreTests);
    
    close(fd);
    return 0;
}
