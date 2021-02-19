#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "libTestsPCR/lectureEcriture.h"

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s [<programme 1> [<programme 2>]]\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    char* test = "";
    if (argc != 3) usage(argv[0]);

    long fd0 = strtol(argv[1], NULL, 10);
    long fd1 = strtol(argv[2], NULL, 10);
    dup2(fd0, 0);
    dup2(fd1, 1);
    while(strlen(test) == 0){
        test = litLigne(fd0);        
    }
    printf("%s", test);     
    return 0;
}
