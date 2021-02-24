#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libTestsPCR/alea.h"
#include "libTestsPCR/lectureEcriture.h"

void usage(char* basename) {
    fprintf(stderr,
        "Utilisation : %s <nombre de tests PCR par centre>\n",
        basename);
    exit(1);
}

void numerosTestsPCR(char* centre, int fd, long nombreTests){
    char* buffer = NULL;
    int aleatest;
    aleainit();
    for(int i = 0; i<nombreTests; i++){
        ecritLigne(fd, centre);
        for(int i = 0; i<3; i++){
            aleatest = alea(0, 9999);
            buffer = malloc(TAILLEBUF);
            sprintf(buffer, "%d", aleatest);
            ecritLigne(fd, buffer);
        }
        ecritLigne(fd, "\n");
    }
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
    return 0;
}
