#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structures.h"
#include "generateur.h"
#include "init.h"
#include "chemins_gloutons.h"
#include "enchere.h"
#include "brute_force.h"

// Tous les camions de l'entrepot a retourne à leurs positions initiales (l'id de l'entrepot a)
entrepot retour_a_la_casa(entrepot a, int **graphe)
{
	for (int i = 0; i < a.nb_camion; i++)
	{
		int taille = a.liste_camion[i]->taille_trajet;
		int origine = a.liste_camion[i]->trajet[taille - 1];
		a.benefice_total -= faire_course(a.liste_camion[i], origine, a.id_entrepot, graphe, 0);
	}
	return a;
}

entrepot le_deficit_ou_pas(entrepot a, int **graphe)
{
	requete *actuelle = a.LR->prem;
	while(actuelle)
	{
		if(actuelle->a_vendre){
			a.benefice_total -= actuelle->perte;
		}
		actuelle = actuelle->suiv;
	}

	return a;
}

int main(int argc, char **argv)
{

	int nb_entrepots = 0;
	char nomfic[64] = "gestionnaire";
	printf("recuperation des information sur le graphe dans le fichier matrice_distance.csv...\n");
	int **graphe;
	if(argc == 2)
		graphe = charge_graphe(argv[1], &nb_entrepots);
	else
		graphe = charge_graphe("matrice_distance.csv", &nb_entrepots);

	genere_acteur(nomfic, graphe, nb_entrepots);
	struct entrepot *a = NULL;
	printf("recuperation des informations sur les entrepots dans le fichier %s...\n", nomfic);
	a = charge_entrepots(nomfic, graphe);

	
	assignation_requete(a[0], graphe);

	for (int i = 0; i < nb_entrepots; i++)
		libere_acteur(a[i]);
	free(a);

	for (int i = 0; i < nb_entrepots; i++)
	{
		free(graphe[i]);
	}
	free(graphe);

	return 0;

	printf("//////////////////////////////////////////////\n");

	printf("chargement des requêtes que les acteurs ne veulent pas dans le dépot commun\n");

	printf("On va maintenant assigner les requêtes de chaque acteur à chaque camion avec l'aide d'un algo glouton : \n");

	char buffer[64];
	printf("Quel type de glouton voulez-vous utiliser, ajout à la fin ou par insertion ? (0/1) \n");
	fflush(stdout);
	scanf("%[^\n]", buffer);
	fgetc(stdin);

	if (buffer[0] == '0')
	{
		printf("On va maintenant faire l'ajout de requête à la fin d'un trajet existant\n");
		for (int i = 0; i < nb_entrepots; i++)
			if (a[i].nb_requete)
				a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete, graphe);

		printf("Souhaitez vous utiliser l'enchère ? (y/n)\n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
		if (buffer[0] == 'y')
		{
			requete ** liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = enchere_echange_fin(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		for (int i = 0; i < nb_entrepots; i++)
		{
			affichage_entrepot(a[i], graphe);
			a[i] = retour_a_la_casa(a[i], graphe);
			a[i] = le_deficit_ou_pas(a[i], graphe);
		}
	}
	else if(buffer[0] == '1')
	{
		printf("On va maintenant faire l'ajout de requête par insertion\n");
		for (int i = 0; i < nb_entrepots; i++)
			if (a[i].nb_requete)
				a[i] = init_insertion(a[i].LR, a[i], a[i].nb_requete, graphe);

		printf("Souhaitez vous utiliser l'enchère ? (y/n)\n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
		if (buffer[0] == 'y')
		{
			requete ** liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = enchere_echange_insertion(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}

		for (int i = 0; i < nb_entrepots; i++)
			a[i] = le_deficit_ou_pas(a[i], graphe);
	}

	analyse_donnees(a, nb_entrepots);

	printf("Souhaitez vous voir le trajet que chaque camion à fait ? (y/n) \n");
	fflush(stdout);
	scanf("%[^\n]", buffer);
	fgetc(stdin);
	while (buffer[0] == 'y')
	{
		memset(buffer, 0, 64);
		printf("Quel acteur souhaitez vous observer ? Entrez son id :\n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
		int id = atoi(buffer);
		for (int i = 0; i < a[id].nb_camion; i++)
		{
			printf("trajet du camion %d : ", a[id].liste_camion[i]->id_camion);
			for (int j = 0; j < a[id].liste_camion[i]->taille_trajet; j++)
				printf("-%d", a[id].liste_camion[i]->trajet[j]);

			printf("-\ncharge du camion %d : ", a[id].liste_camion[i]->id_camion);
			for (int j = 0; j < a[id].liste_camion[i]->taille_trajet - 1; j++)
				printf("-%d", a[id].liste_camion[i]->charge[j]);
			printf("-\n");
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