#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "../libTestsPCR/lectureEcriture.h"
#include "../libTestsPCR/message.h"

void usage(char * basename) {
    fprintf(stderr,
        "Utilisation : %s <descripteur de fichier (entrée)> <descripteur de fichier (sortie)> <nom du fichier des résultats des tests PCR>\n",
        basename);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc != 4) usage(argv[0]);

    long entree = strtol(argv[1], NULL, 10);
    long sortie = strtol(argv[2], NULL, 10);
    
    char* nTest = malloc(50);
    char* type = malloc(50);
    char* valeur = malloc(50);
    char* nomFichier = argv[3]; 
    char* buffer = malloc(TAILLEBUF);

    while(1){
        int fd = open(nomFichier, O_RDONLY);
        long nombreTests = strtol(litLigne(fd), NULL, 10);
        
        buffer = litLigne(entree);
        if(!decoupe(buffer, nTest, type, valeur)){
            perror("Erreur de la fonction decoupe");
            exit(-1);
        }
        if(strcmp(type, "Demande") != 0){
            ecritLigne(1, "Mauvais type de message. \n");
            exit(-1);
        }
        printf("test");
        for(int i = 0; i < nombreTests; i++){
            buffer = strtok(litLigne(fd), " " );
            if(strcmp(buffer, nTest) == 0){
                buffer = strtok(NULL, " " );
                int date = time(NULL); 
                date = date - strtol(buffer, NULL, 10);
                if(date < strtol(valeur, NULL, 10) * 3600 && strcmp(strtok(NULL, " " ), "Négatif") == 0){
                    buffer = message(nTest, "Reponse", "1");
                    ecritLigne(sortie, buffer);
                } else{
                    buffer = message(nTest, "Reponse", "0");
                    ecritLigne(sortie, buffer);
                }
                break;
            }
        }
        close(fd);
    }

    free(nTest);
    free(type);
    free(valeur);
    free(buffer);
    close(entree);
    close(sortie);
    return 0;
}