#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libTestsPCR/alea.h"
#include "libTestsPCR/lectureEcriture.h"

int main(int argc, char *argv[]) {
    int fd = open("Numeros_tests_PCR.txt", O_CREAT | O_WRONLY, 0644);
    aleainit();
    int aleatest = alea(0, 99);
    char* buffer = NULL;
    buffer = malloc(TAILLEBUF);
    sprintf(buffer, "%d", aleatest);
    ecritLigne(fd, buffer);
    return 0;
}
