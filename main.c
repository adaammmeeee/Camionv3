#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "string.h"

int main()
{
	struct entrepot *a = NULL;
	int nb_entrepots;
	printf("recuperation des informations sur les entrepots dans le fichier gestionnaire...\n");
	a = charge_entrepots("gestionnaire", &nb_entrepots);

	printf("recuperation des information sur le graphe dans le fichier matrice_distance.csv...\n");
	int **graphe = charge_graphe("matrice_distance.csv", nb_entrepots);

	printf("On va maintenant assigner les requêtes de chaque acteur à chaque camion avec l'aide d'un algo glouton : \n");
	for (int i = 0; i < nb_entrepots; i++)
	{
		printf("rentabilité de l'acteur %c : %d\n", a[i].id_entrepot, evaluation_meilleure_solution(a[i], a[i].nb_camion, graphe));
	}

	char buffer[2];
	printf("Souhaitez vous voir le trajet que chaque camion à fait ? (y/n) \n");
	fflush(stdout);
	scanf("%[^\n]", buffer);
	fgetc(stdin);
	while (buffer[0] == 'y')
	{
		memset(buffer, 0, 2);
		printf("Quel acteur souhaitez vous observer ? Entrez son id :\n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
		int id = buffer[0] - 'A';
		for (int i = 0; i < a[id].nb_camion; i++)
		{
			printf("trajet du camion : %s\n", a[id].liste_camion[i]->trajet);
		}
		printf("Souhaitez vous voir les trajets d'un autre acteur ? (y/n) \n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
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
