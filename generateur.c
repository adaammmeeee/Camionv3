#include "structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int genere_acteur(char *nomfic, float **graphe, int nb_entrepots)
{
    
    FILE *f = fopen(nomfic, "w");
    if (f == NULL)
    {
        printf("Erreur d'ouverture du fichier\n");
        return -1;
    }
    fprintf(f, "nombre d'entrepot : %d\n\n", nb_entrepots);

    srand(getpid());

    for (int i = 0; i < nb_entrepots; i++)
    {
        int nombre_requetes = rand() % 20 + 1; // entre 300 et 2000 requetes
        int nombre_camions =  8; // 10 requetes par camion
        fprintf(f, "entrepot : %d\nnombre de camion : %d\nnombre de requete : %d\n\n", i, nombre_camions, nombre_requetes);

        for (int j = 0; j < nombre_requetes; j++)
        {
            int origine = i;
            int destination = rand() % nb_entrepots;
            while (destination == origine)
            {
                destination = rand() % nb_entrepots;
            }
            float gain = cout_distance(graphe[origine][destination])*10 + rand() % 1000;
            float perte = 100;

            fprintf(f, "origine : %d\ndestination : %d\ngains : %f\nperte : %f\n\n", origine, destination, gain, perte);
        }
    }

    fclose(f);
    return 0;
}