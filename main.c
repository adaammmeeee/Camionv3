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
		printf("NOUVEL entrepot ---------------------------------------------------------------\n");
		affichage(a[i].LR);
	}

	int graphe[3][3] = {{9, 5, 3},
						{4, 7, 2},
						{4, 3, 9}};



	for (int i = 0; i < 2; i++)
		liberation(a[i].LR);
	free(a);

	return 0;
}
