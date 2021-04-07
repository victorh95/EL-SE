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

void usage(char* basename) {
    fprintf(stderr,
        "Utilisation : %s <nombre de centres (entre 1 et 3)> <nombre de tests PCR par centre>\n",
        basename);
    exit(1);
}

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
        if(aleaResultat == 0) strcat(buffer, " Positif \n");
        else strcat(buffer, " Négatif \n");
        ecritLigne(fd1, buffer);
    }
    
    free(nomFichier);
    free(buffer);
    free(testDate);
    close(fd1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage(argv[0]);

    int fd = open("Numeros_tests_PCR.txt", O_RDONLY);
    litLigne(fd);
    long nombreCentres = strtol(argv[1], NULL, 10);
    strcat(argv[2], "\n");

    switch(nombreCentres){
        case 1:
            //Paris
            numerosTestsPCRcentre("Paris", fd, argv[2]);
            break;
        case 2:
            //Paris
            numerosTestsPCRcentre("Paris", fd, argv[2]);
            //Nice
            numerosTestsPCRcentre("Nice", fd, argv[2]);
            break;
        case 3:
            //Paris
            numerosTestsPCRcentre("Paris", fd, argv[2]);
            //Nice
            numerosTestsPCRcentre("Nice", fd, argv[2]);
            //Madrid
            numerosTestsPCRcentre("Madrid", fd, argv[2]);
            break;
        default:
            break;
    }

    close(fd);
    return 0;
}
