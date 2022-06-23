#include "structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int genere_acteur(char *nomfic, int **graphe, int nb_entrepots)
{

    FILE *f = fopen(nomfic, "w");
    if (f == NULL)
    {
        printf("Erreur d'ouverture du fichier\n");
        return -1;
    }
    fprintf(f, "nombre d'entrepot : %d\n\n", nb_entrepots);

    //srand(getpid());
    srand(3);

    for (int i = 0; i < nb_entrepots; i++)
    {
        int nombre_requetes = 10; // entre 300 et 2000 requetes
        int nombre_camions = 7;   // 10 requetes par camion
        int destination = 0 ;
        fprintf(f, "entrepot : %d\nnombre de camion : %d\nnombre de requete : %d\n\n", i, nombre_camions, nombre_requetes);
        int gain = 0;
        for (int j = 0; j < nombre_requetes; j++)
        {
            int origine =  destination;
            destination = rand() % nb_entrepots;
            while (destination == origine)
            {
                destination = rand() % nb_entrepots;
            }
            gain = cout_distance(graphe[origine][destination]);
            gain = gain * 2.5;
            int perte = gain * 10;

            fprintf(f, "origine : %d\ndestination : %d\ngains : %d\nperte : %d\n\n", origine, destination, gain, perte);
        }
    }

    fclose(f);
    return 0;
}




