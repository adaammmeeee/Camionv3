#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "string.h"

int main()
{

	int nb_entrepots = charge_nombre_entrepots("gestionnaire");

	printf("recuperation des information sur le graphe dans le fichier matrice_distance.csv...\n");
	int **graphe = charge_graphe("matrice_distance.csv", nb_entrepots);

	struct entrepot *a = NULL;
	printf("recuperation des informations sur les entrepots dans le fichier gestionnaire...\n");
	a = charge_entrepots("gestionnaire", graphe);

	printf("chargement des requêtes que les acteurs ne veulent pas dans le dépot commun\n");
	requete liste_vente[nb_entrepots];

	for (int i = 0; i < nb_entrepots; i++)
	{
		if (a[i].nb_requete)
			liste_vente[i] = copie_requete(a[i].LR->dern);
	}

	printf("On va maintenant assigner les requêtes de chaque acteur à chaque camion avec l'aide d'un algo glouton : \n");
	for (int i = 0; i < nb_entrepots; i++)
	{
		printf("rentabilité de l'acteur %c : %d\n", a[i].id_entrepot, evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete, graphe));
	}

	requete *nouv = malloc(sizeof(struct requete));
	nouv->origine = 'A';
	nouv->destination = 'B';
	nouv->prec = NULL;
	nouv->suiv = NULL;
	nouv->gain = 350;
	nouv->perte = 600;

	printf("voici le cout de la requete à la fin :%d\n", cout_requete_fin_trajet(nouv, a[0], graphe));

	free(nouv);

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
