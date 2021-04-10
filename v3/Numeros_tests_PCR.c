#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../libTestsPCR/lectureEcriture.h"
#include "../libTestsPCR/alea.h"

void usage(char* basename) {
    fprintf(stderr,
        "Utilisation : %s <nombre de centres (entre 1 et 3)> <nombre de tests PCR par centre>\n",
        basename);
    exit(1);
}

void numerosTestsPCR(char* centre, int fd, long nombreTests){
    char* buffer = malloc(TAILLEBUF);
    int alea4;
    aleainit();
    for(int i = 0; i<nombreTests; i++){
        ecritLigne(fd, centre);
        for(int i = 0; i<3; i++){
            alea4 = alea(0, 9999);
            sprintf(buffer, "%d", alea4);
            switch(strlen(buffer)){
                case 1 :
                    strcat(buffer, "000");
                    break;
                case 2 :
                    strcat(buffer, "00");
                    break;
                case 3 :
                    strcat(buffer, "0");
                    break;
                default :
                    break;
            }
            ecritLigne(fd, buffer);
        }
        ecritLigne(fd, "\n");
    }
    free(buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 3) usage(argv[0]);

    int fd = open("Numeros_tests_PCR.txt", O_CREAT | O_WRONLY, 0644);  
    long nombreCentres = strtol(argv[1], NULL, 10);
    long nombreTests = strtol(argv[2], NULL, 10);
    char* nTotalTests = malloc(100);

    switch(nombreCentres){
        case 1:
            sprintf(nTotalTests, "%ld \n", nombreTests);
            ecritLigne(fd, nTotalTests);
            //Paris
            numerosTestsPCR("0000", fd, nombreTests);
            printf("Le centre de Paris a été crée, son code est : \"0000\".\n");
            break;
        case 2:
            sprintf(nTotalTests, "%ld \n", 2*nombreTests);
            ecritLigne(fd, nTotalTests);
            //Paris
            numerosTestsPCR("0000", fd, nombreTests);
            //Nice
            numerosTestsPCR("1111", fd, nombreTests);
            printf("Les centres de Paris et Nice ont été crées, respectivement, leurs codes sont : \"0000\" et \"1111\".\n");
            break;
        case 3:
            sprintf(nTotalTests, "%ld \n", 3*nombreTests);
            ecritLigne(fd, nTotalTests);
            //Paris
            numerosTestsPCR("0000", fd, nombreTests);
            //Nice
            numerosTestsPCR("1111", fd, nombreTests);
            //Madrid
            numerosTestsPCR("2222", fd, nombreTests);
            printf("Les centres de Paris, Nice et Madrid ont été crées, respectivement, leurs codes sont : \"0000\", \"1111\" et \"2222\".\n");
            break;
        default:
            printf("Vous devez choisir un nombre entre 1 et 3.\n");
            break;
    }
    
    free(nTotalTests);
    close(fd);

    execlp("./Numeros_tests_PCR_centre", "./Numeros_tests_PCR_centre", argv[1], argv[2], NULL);
    
    return 0;
}
