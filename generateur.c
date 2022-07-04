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
        int comparateur = rand()%100;
        int nombre_requetes;
        if (comparateur < 85)
        { 
            nombre_requetes = rand()%8+3; // Petit acteur
        }
        else
        {
            nombre_requetes = rand()%8 + 3; // Grand acteur
        }
        int nombre_camions = nombre_requetes/2 + 1;   // 2 requetes par camion
        int destination = 0 ;
        fprintf(f, "entrepot : %d\nnombre de camion : %d\nnombre de requete : %d\n\n", i, nombre_camions, nombre_requetes);
        int gain = 0;
        for (int j = 0; j < nombre_requetes; j++)
        {
            int origine =  i;
            destination = rand() % nb_entrepots;
            while (destination == origine || graphe[origine][destination] > (1000*1000)) // 1000km en mètre
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




