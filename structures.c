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

void affichage_requete(liste_requete *LR, int **graphe)
{
	requete *actuelle = LR->prem;
	while (actuelle)
	{
		printf("entrepot : %d requete : %d->%d\nGains : %.2f Perte : %.2f\nDistance : %.2f Cout : %.2f Benefice : %.2f\n\n",
			   actuelle->id_entrepot, actuelle->origine, actuelle->destination, (float) actuelle->gain/10000, (float) actuelle->perte/10000, 
               (float) graphe[actuelle->origine][actuelle->destination]/10000, (float) cout_distance(graphe[actuelle->origine][actuelle->destination])/10000, 
               (float) (actuelle->gain - cout_distance(graphe[actuelle->origine][actuelle->destination]))/10000);
		actuelle = actuelle->suiv;
	}
}

void affichage_entrepot(entrepot a, int **graphe)
{
	printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
	for (int i = 0; i < a.nb_camion; i++)
	{
		printf("\nid_camion : %d\ndistance_parcouru : %d\nTrajet effectué :", i, a.liste_camion[i]->distance_parcouru);
		for (int j = 0; j < a.liste_camion[i]->taille_trajet; j++)
			printf("-%d-", a.liste_camion[i]->trajet[j]);
		printf("-\n");
	}
	affichage_requete(a.LR, graphe);
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

		somme_m += (float) a[i].gain_total/10000;
		somme_var = somme_var + ((float) a[i].gain_total/10000) * ((float) a[i].gain_total/10000);
		printf("rentabilité de l'acteur %d : %.2f\n", a[i].id_entrepot, (float) a[i].gain_total/10000);
	}
	float moyenne = somme_m/nb_entrepots;
	float variance = somme_var/nb_entrepots-moyenne*moyenne;
	float ecart_type = sqrt(variance);
	printf("\nGain global %.2f, moyenne par acteur : %.2f\n", somme_m, moyenne);
	printf("Le gain le plus haut : %.2f, le gain le plus bas : %.2f et un ecart-type : %.2f\n\n", (float) max/10000, (float) min/10000, ecart_type);
}
