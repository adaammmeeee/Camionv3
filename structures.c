#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "structures.h"

int cout_distance(int distance)
{
	if (distance)
		return 11 * distance + 500000;
	else
		return 0;
}

void affichage_requete(liste_requete *LR, int **graphe)
{
	requete *actuelle = LR->prem;
	while (actuelle)
	{
		printf("entrepot : %d requete : %d->%d\nGains : %.2f Perte : %.2f\nDistance : %.2f\nVendue ? %s\n\n",
			   actuelle->id_entrepot, actuelle->origine, actuelle->destination, (float)actuelle->gain / 10000, (float)actuelle->perte / 10000,
			   (float)graphe[actuelle->origine][actuelle->destination] / 1000, actuelle->a_vendre ? "non" : "oui");
		actuelle = actuelle->suiv;
	}
}

void affichage_entrepot(entrepot a, int **graphe)
{
	int cout_entrepot = 0;
	int gain_theorique = 0;
	printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
	for (int i = 0; i < a.nb_camion; i++)
	{
		int cout_trajet_camion = 0;
		printf("\nid_camion : %d\ndistance_parcouru : %.2f\nTrajet effectué :", i, (float)a.liste_camion[i]->distance_parcouru / 1000);
		for (int j = 0; j < a.liste_camion[i]->taille_trajet; j++)
		{
			printf("-%d", a.liste_camion[i]->trajet[j]);
			if (j < a.liste_camion[i]->taille_trajet - 1)
			{
				cout_trajet_camion += cout_distance(graphe[a.liste_camion[i]->trajet[j]][a.liste_camion[i]->trajet[j + 1]]);
			}
		}
		printf("-\nCout_trajet %.2f\n", (float)cout_trajet_camion / 10000);
		cout_entrepot += cout_trajet_camion;
	}
	requete *actuelle = a.LR->prem;
	while (actuelle)
	{
		if (!actuelle->a_vendre)
			gain_theorique += actuelle->gain;
		actuelle = actuelle->suiv;
	}
	printf("-\nGain théorique %.2f, Cout_total %.2f, Benefice : %.2f\n", (float)gain_theorique / 10000, (float)cout_entrepot / 10000, (float)a.benefice_total / 10000);
	affichage_requete(a.LR, graphe);
}
// type enchere : 0 = pas d'enchère, 1 = enchère, 2 = appel confiance
void analyse_donnees(entrepot *a, int nb_entrepot, int type_enchere)
{
	float nb_entrepots = (float)nb_entrepot;
	float somme_m = 0;
	float somme_var = 0;
	float min = INT_MAX;
	float max = INT_MIN;
	char nomfic[255];
	sprintf(nomfic, "analyse%d", type_enchere);
	FILE *fichier = fopen(nomfic, "a");
	if (!fichier)
	{
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}

	for (int i = 0; i < nb_entrepots; i++)
	{
		if (a[i].benefice_total < min)
			min = a[i].benefice_total;
		if (a[i].benefice_total > max)
			max = a[i].benefice_total;

		somme_m += (float)a[i].benefice_total / 10000;
		somme_var = somme_var + ((float)a[i].benefice_total / 10000) * ((float)a[i].benefice_total / 10000);

		fprintf(fichier, "acteur %d : %.2f\n", a[i].id_entrepot, (float)a[i].benefice_total / 10000);
	}
	float moyenne = somme_m / nb_entrepots;
	float variance = somme_var / nb_entrepots - moyenne * moyenne;
	float ecart_type = sqrt(variance);
	fprintf(fichier,"moyenne par acteur : %.2f\n", moyenne);
	fprintf(fichier,"gain max : %.2f\n", (float) max / 10000);
	fprintf(fichier,"gain min : %.2f\n", (float) min / 10000);
	fprintf(fichier,"ecart-type : %.2f\n", ecart_type);
	fclose(fichier);
}

void exporte_trajet(entrepot *a, int nb_entrepot)
{

	FILE *f = fopen("trajet", "w");
	if (f == NULL)
	{
		printf("Erreur d'ouverture du fichier\n");
		return;
	}
	for (int i = 0; i < nb_entrepot; i++)
	{
		fprintf(f, "Entrepot : %d\n", a[i].id_entrepot);
		fprintf(f, "Nombre de camion : %d\n", a[i].nb_camion);
		for (int j = 0; j < a[i].nb_camion; j++)
		{
			fprintf(f, "Camion : %d\n", j);
			fprintf(f, "Distance parcourue : %.2f\n", (float)a[i].liste_camion[j]->distance_parcouru / 1000);
			fprintf(f, "Trajet : ");
			for (int k = 0; k < a[i].liste_camion[j]->taille_trajet; k++)
			{
				fprintf(f, "%d", a[i].liste_camion[j]->trajet[k]);
				if (k < a[i].liste_camion[j]->taille_trajet - 1)
					fprintf(f, "-");
			}
			fprintf(f, "\n");
			fprintf(f, "Charge : ");
			for (int k = 0; k < a[i].liste_camion[j]->taille_trajet - 1; k++)
			{
				fprintf(f, "%d", a[i].liste_camion[j]->charge[k]);
				if (k < a[i].liste_camion[j]->taille_trajet - 2)
					fprintf(f, "-");
			}
			fprintf(f, "\n");
		}
	}
	fclose(f);
}