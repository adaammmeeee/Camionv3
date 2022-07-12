#include "structures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void genere_alea(int nb_entrepots, int *nb_requetes, int *nb_camions)
{
    srand(3);

    for (int i = 0; i < nb_entrepots; i++)
    {
        int comparateur = rand()%100;
        if (comparateur < 85)
        { 
            nb_requetes[i] = rand()%7+3; // Petit acteur
        }
        else
        {
            nb_requetes[i] = rand()%70 + 20; // Grand acteur
        }
        nb_camions[i] = nb_requetes[i]/2 + 1;   // 2 requetes par camion
    }
}

int genere_acteur(char *nomfic, int **graphe, int nb_entrepots, int *nb_requetes, int *nb_camions)
{

    FILE *f = fopen(nomfic, "w");
    if (f == NULL)
    {
        printf("Erreur d'ouverture du fichier\n");
        return -1;
    }
    fprintf(f, "nombre d'entrepot : %d\n\n", nb_entrepots);

    for (int i = 0; i < nb_entrepots; i++)
    {
        fprintf(f, "entrepot : %d\nnombre de camion : %d\nnombre de requete : %d\n\n", i, nb_camions[i], nb_requetes[i]);

        int destination = 0 ;
        int gain = 0;
        for (int j = 0; j < nb_requetes[i]; j++)
        {
            int origine =  i;
            destination = rand()%nb_entrepots;
            while (destination == origine || graphe[origine][destination] > (1000*1000)) // 1000km en mètre
            {
                destination = rand() % nb_entrepots;
            }
            gain = cout_distance(graphe[origine][destination]);
            gain = gain * 2.5;
            int perte = gain;

            fprintf(f, "origine : %d\ndestination : %d\ngains : %d\nperte : %d\n\n", origine, destination, gain, perte);
        }
    }

    fclose(f);
    return 0;
}
