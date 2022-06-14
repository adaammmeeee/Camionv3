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

	printf("On va maintenant assigner les requêtes de chaque acteur à chaque camion avec l'aide d'un algo glouton : \n");

	char buffer[2];
	printf("Souhaitez vous utilisé l'enchère ? (y/n)\n");
	fflush(stdout);
	scanf("%[^\n]", buffer);
	fgetc(stdin);

	if (buffer[0] == 'n')
	{
		printf("On procède sans enchères\n");
		for (int i = 0; i < nb_entrepots; i++)
		{
			if (a[i].nb_requete)
				a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete, graphe);
		}
	}
	else if (buffer[0] == 'y')
	{
		printf("On va maintenant faire l'enchère\n");
		int nb_requete_vente = 0;
		for (int i = 0; i < nb_entrepots; i++)
		{
			if (a[i].nb_requete)
			{
				a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete - 1, graphe);
				int camion = -1;
				int cout_requete = cout_requete_fin_trajet(*(a[i].LR->dern), a[i], &camion, graphe);
				liste_vente[i] = copie_requete(*(a[i].LR->dern), cout_requete);
				nb_requete_vente++;
			}
		}
		a = enchere_echange(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
	}
	else
	{
		printf("Je ne comprend pas ce que tu me demandes, j'arrête\n");
	}

	for (int i = 0; i < nb_entrepots; i++)
		a[i] = retour_a_la_casa(a[i], graphe);

	for (int i = 0; i < nb_entrepots; i++)
		printf("rentabilité de l'acteur %c : %d\n", a[i].id_entrepot, a[i].gain_total);

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
			printf("charge du camion : %s\n", a[id].liste_camion[i]->charge);
		}
		printf("Souhaitez vous voir les trajets d'un autre acteur ? (y/n) \n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
	}
	//////////////////////// On test la fonction insertion
	printf("\n\n\nTest insertion\n");
	int indice_trajet;
	char id_camion[2];
	memset(id_camion, 0, 2);

	requete nouv;
	nouv.origine = 'A';
	nouv.destination = 'B';
	nouv.prec = NULL;
	nouv.suiv = NULL;
	nouv.gain = 350;
	nouv.perte = 600;

	insertion(&indice_trajet, id_camion, a[0], nouv, graphe);
	printf("Si on voulait insérer une requete %c->%c ayant pour gain %d\nAlors on la confierai au camion %s et elle serait incrusté à l'étape %d \n", nouv.origine, nouv.destination, nouv.gain, id_camion, indice_trajet);

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