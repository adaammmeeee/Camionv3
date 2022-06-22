#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "generateur.h"
#include "init.h"
#include "chemins_gloutons.h"
#include "enchere.h"
#include "brute_force.h"

void affichage_requete(liste_requete *LR)
{
	requete *actuelle = LR->prem;
	while (actuelle)
	{
		printf("origine : %d\ndestination : %d\ngains : %.2f\nperte : %.2f\n\n",
			   actuelle->origine, actuelle->destination, actuelle->gain, actuelle->perte);
		actuelle = actuelle->suiv;
	}
}

void affichage_entrepot(entrepot a)
{
	printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
	for (int i = 0; i < a.nb_camion; i++)
	{
		printf("\nid_camion : %d\ndistance_parcouru : %.2f\nTrajet effectué :", i, a.liste_camion[i]->distance_parcouru);
		for (int j = 0; j < a.liste_camion[i]->taille_trajet; j++)
			printf("-%d-", a.liste_camion[i]->trajet[j]);
		printf("-\n");
	}
	affichage_requete(a.LR);
}

// Tous les camions de l'entrepot a retourne à leurs positions initiales (l'id de l'entrepot a)
entrepot retour_a_la_casa(entrepot a, float **graphe)
{
	for (int i = 0; i < a.nb_camion; i++)
	{
		int taille = a.liste_camion[i]->taille_trajet;
		int origine = a.liste_camion[i]->trajet[taille - 1];
		a.gain_total -= faire_course(a.liste_camion[i], origine, a.id_entrepot, graphe, 0);
	}
	return a;
}

int main(int argc, char **argv)
{

	int nb_entrepots = 0;
	char nomfic[64] = "gestionnaire";
	printf("recuperation des information sur le graphe dans le fichier matrice_distance.csv...\n");
	float **graphe;
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
	//affichage_entrepot(a[1]);*/

	printf("chargement des requêtes que les acteurs ne veulent pas dans le dépot commun\n");
	requete liste_vente[nb_entrepots];

	printf("On va maintenant assigner les requêtes de chaque acteur à chaque camion avec l'aide d'un algo glouton : \n");

	char buffer[64];
	printf("Souhaitez vous utilisé l'enchère ? (y/n)\n");
	fflush(stdout);
	scanf("%[^\n]", buffer);
	fgetc(stdin);

	if (buffer[0] == 'n')
	{
		printf("On procède sans enchères\n");
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

			for (int i = 0; i < nb_entrepots; i++)
				a[i] = retour_a_la_casa(a[i], graphe);
		}
		else if(buffer[0] == '1')
		{
			printf("On va maintenant faire l'ajout de requête par insertion\n");
			for (int i = 0; i < nb_entrepots; i++)
				if (a[i].nb_requete)
					a[i] = init_insertion(a[i].LR, a[i], a[i].nb_requete, graphe);
		}
	}
	else if (buffer[0] == 'y')
	{
		printf("Quel type de glouton voulez-vous utiliser, ajout à la fin ou par insertion ? (0/1) \n");
		fflush(stdout);
		scanf("%[^\n]", buffer);
		fgetc(stdin);
		if (buffer[0] == '0')
		{
			printf("On va maintenant faire l'ajout de requête à la fin d'un trajet existant\n");
			int nb_requete_vente = 0;
			for (int i = 0; i < nb_entrepots; i++)
			{
				if (a[i].nb_requete > 1)
				{
					a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete - 1, graphe);
					int camion = -1;
					int cout_requete = cout_requete_fin_trajet(*(a[i].LR->dern), a[i], &camion, graphe);
					if(camion == -1 && cout_requete)
						return 1;
					else if(!cout_requete)
						cout_requete = a[i].LR->dern->perte - 1;

					liste_vente[nb_requete_vente] = copie_requete(*(a[i].LR->dern), cout_requete);
					nb_requete_vente++;
				}
			}
			if (nb_requete_vente)
				a = enchere_echange_fin(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);

			for (int i = 0; i < nb_entrepots; i++)
				a[i] = retour_a_la_casa(a[i], graphe);
		}
		else if (buffer[0] == '1')
		{
			printf("On va maintenant faire l'ajout de requête par insertion\n");
			int nb_requete_vente = 0;
			int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
			int *new_charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));
			for (int i = 0; i < nb_entrepots; i++)
			{
				if (a[i].nb_requete > 1)
				{
					a[i] = init_insertion(a[i].LR, a[i], a[i].nb_requete - 1, graphe);
					memset(new_trajet, 0, TAILLE_MAX_TRAJET);
					memset(new_trajet, 0, TAILLE_MAX_TRAJET - 1);
					int camion = -1; int taille_new_trajet = 0;
					int cout_requete = insertion(*(a[i].LR->dern), a[i], &camion, new_trajet, new_charge, &taille_new_trajet, graphe);
					if((camion == -1 || !taille_new_trajet)&& cout_requete)
						return 1;
					else if(!cout_requete)
						cout_requete = a[i].LR->dern->perte - 1;

					liste_vente[nb_requete_vente] = copie_requete(*(a[i].LR->dern), cout_requete);
					nb_requete_vente++;
				}
			}
			free(new_trajet);
			free(new_charge);

			if(nb_requete_vente)
				a = enchere_echange_insertion(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		else
			printf("Je ne comprend pas ce que tu me demandes, arrête\n");
	}
	else
		printf("Je ne comprend pas ce que tu me demandes, arrête\n");

	for (int i = 0; i < nb_entrepots; i++)
		printf("rentabilité de l'acteur %d : %.2f\n", a[i].id_entrepot, a[i].gain_total);

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