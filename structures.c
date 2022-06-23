#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structures.h"

int cout_distance(int distance)
{
    if(distance)
        return 110*distance + 5000;
    else
        return 0;
}

void affichage_requete(liste_requete *LR)
{
	requete *actuelle = LR->prem;
	while (actuelle)
	{
		printf("origine : %d\ndestination : %d\ngains : %d\nperte : %d\n\n",
			   actuelle->origine, actuelle->destination, actuelle->gain, actuelle->perte);
		actuelle = actuelle->suiv;
	}
}

void affichage_entrepot(entrepot a)
{
	printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
	for (int i = 0; i < a.nb_camion; i++)
	{
		printf("\nid_camion : %d\ndistance_parcouru : %d\nTrajet effectué :", i, a.liste_camion[i]->distance_parcouru);
		for (int j = 0; j < a.liste_camion[i]->taille_trajet; j++)
			printf("-%d-", a.liste_camion[i]->trajet[j]);
		printf("-\n");
	}
	affichage_requete(a.LR);
}

void analyse_donnees(entrepot *a, int nb_entrepot)
{
	float nb_entrepots = (float) nb_entrepot;
	float somme_m = 0;
	float somme_var = 0;
	float min = MAX;
	float max = 0;
	for (int i = 0; i < nb_entrepots; i++)
	{
		if(a[i].gain_total < min)
			min = a[i].gain_total;
		if(a[i].gain_total > max)
			max = a[i].gain_total;

		somme_m += a[i].gain_total;
		somme_var = somme_var + a[i].gain_total * a[i].gain_total;
		printf("rentabilité de l'acteur %d : %d\n", a[i].id_entrepot, a[i].gain_total);
	}
	float moyenne = somme_m/nb_entrepots;
	float variance = somme_var/nb_entrepots-moyenne*moyenne;
	float ecart_type = sqrt(variance);
	printf("\nGain global %.2f, moyenne par acteur : %.2f\n", somme_m, moyenne);
	printf("Le gain le plus haut : %.2f, le gain le plus bas : %.2f et un ecart-type : %.2f\n\n", max, min, ecart_type);
}
