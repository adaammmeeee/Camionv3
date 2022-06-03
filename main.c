#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

int main()
{
	struct entrepot *a = NULL;
	int nb_entrepots;
	a = charge_entrepots("gestionnaire", &nb_entrepots);

	for (int i = 0; i < 2; i++)
	{
		// printf("NOUVEL entrepot ---------------------------------------------------------------\n");
		// affichage_entrepot(a[i]);
	}

	int **graphe = malloc(nb_entrepots * sizeof(int *));
	for (int i = 0; i < nb_entrepots; i++)
	{
		graphe[i] = malloc(nb_entrepots * sizeof(int));
	}

	for (int i = 0; i < nb_entrepots; i++)
	{
		for (int j = 0; j < nb_entrepots; j++)
		{
			graphe[i][j] = 10;
		}
	}

	for (int i = 0; i < nb_entrepots;i++)
	{
		course_basique(graphe, a[i]);
	}


	for (int i = 0; i < nb_entrepots; i++)
		libere_acteur(a[i]);
	free(a);

	for (int i = 0; i < nb_entrepots; i++)
	{
		free(graphe[i]);
	}
	free(graphe);

	return 0;
}
