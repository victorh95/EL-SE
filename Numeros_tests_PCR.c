#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libTestsPCR/lectureEcriture.h"
#include "libTestsPCR/alea.h"

void usage(char* basename) {
    fprintf(stderr,
        "Utilisation : %s <nombre de tests PCR par centre>\n",
        basename);
    exit(1);
}

void numerosTestsPCR(char* centre, int fd, long nombreTests){
    char* buffer = NULL;
    int alea4;
    aleainit();
    for(int i = 0; i<nombreTests; i++){
        ecritLigne(fd, centre);
        for(int i = 0; i<3; i++){
            alea4 = alea(0, 9999);
            buffer = malloc(TAILLEBUF);
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
    if (argc != 2) usage(argv[0]);

    int fd = open("Numeros_tests_PCR.txt", O_CREAT | O_WRONLY, 0644);
    long nombresTests = strtol(argv[1], NULL, 10);
    //Paris
    numerosTestsPCR("0000", fd, nombresTests);
    //Nice
    numerosTestsPCR("1111", fd, nombresTests);
    //Madrid
    numerosTestsPCR("2222", fd, nombresTests);
    
    close(fd);
    return 0;
}
