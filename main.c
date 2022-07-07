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
		faire_course(a.liste_camion[i], origine, a.id_entrepot, graphe, 0);
	}
	return a;
}

entrepot le_deficit_ou_pas(entrepot a, int **graphe)
{
	requete *actuelle = a.LR->prem;
	while (actuelle)
	{
		if (actuelle->a_vendre)
		{
			a.benefice_total -= actuelle->perte;
		}

		actuelle = actuelle->suiv;
	}

	for (int i = 0; i < a.nb_camion; i++)
		for (int j = 0; j < a.liste_camion[i]->taille_trajet - 1; j++)
			a.benefice_total -= cout_distance(graphe[a.liste_camion[i]->trajet[j]][a.liste_camion[i]->trajet[j + 1]]);

	return a;
}

int main(int argc, char **argv)
{

	int type_enchere = 0;
	int nb_entrepots = 0;
	int nb_tours = 0;
	int grand_echantillon = 0;
	char nomfic[64] = "gestionnaire";
	int **graphe;
	struct entrepot *a = NULL;
	if (argc < 3)
	{
		printf("besoin d'arguments %s nom_fichier.csv type_algo(fin/insertion/brute)\n", argv[0]);
		return -1;
	}
	else if (argc == 5)
		grand_echantillon = 1;

	graphe = charge_graphe(argv[1], &nb_entrepots);

	int nb_requetes[nb_entrepots];
	int nb_camions[nb_entrepots];
	genere_alea(nb_entrepots, nb_requetes, nb_camions);

repetition:
	nb_tours++;
	genere_acteur(nomfic, graphe, nb_entrepots, nb_requetes, nb_camions);
	if (a)
		free(a);
	a = charge_entrepots(nomfic, graphe);

	if (!strcmp(argv[2], "fin"))
	{
		if (argc < 4)
		{
			for (int i = 0; i < nb_entrepots; i++)
			{
				libere_acteur(a[i]);
				free(graphe[i]);
			}
			free(a);
			free(graphe);

			printf("besoin d'arguments %s nom_fichier.csv type_algo(fin/insertion) type_echanges(sans/confiance/enchere) <option>nb_repetitions\n", argv[0]);
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
			if (a[i].nb_requete)
				a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete, graphe);

		if (!strcmp(argv[3], "confiance"))
		{
			type_enchere = 1;
			requete **liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = confiance_fin(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		else if (!strcmp(argv[3], "enchere"))
		{
			type_enchere = 2;
			requete **liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = enchere_echange_fin(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		else if (strcmp(argv[3], "sans"))
		{
			for (int i = 0; i < nb_entrepots; i++)
			{
				libere_acteur(a[i]);
				free(graphe[i]);
			}
			free(a);
			free(graphe);

			printf("besoin d'arguments %s nom_fichier.csv type_algo(fin/insertion) type_echanges(sans/confiance/enchere) <option>nb_repetitions\n", argv[0]);
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
		{
			a[i] = retour_a_la_casa(a[i], graphe);
			a[i] = le_deficit_ou_pas(a[i], graphe);
		}
	}
	else if (!strcmp(argv[2], "insertion"))
	{
		if (argc < 4)
		{
			for (int i = 0; i < nb_entrepots; i++)
			{
				libere_acteur(a[i]);
				free(graphe[i]);
			}
			free(a);
			free(graphe);

			printf("besoin d'arguments %s nom_fichier.csv type_algo(fin/insertion) type_echanges(sans/confiance/enchere) <option>nb_repetitions\n", argv[0]);
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
			if (a[i].nb_requete)
				a[i] = init_insertion(a[i].LR, a[i], a[i].nb_requete, graphe);

		if (!strcmp(argv[3], "confiance"))
		{
			type_enchere = 1;
			requete **liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = confiance_insertion(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		else if (!strcmp(argv[3], "enchere"))
		{
			type_enchere = 2;
			requete **liste_vente;
			int nb_requete_vente = 0;
			liste_vente = mise_en_vente(a, nb_entrepots, &nb_requete_vente);

			if (nb_requete_vente && liste_vente)
				a = enchere_echange_insertion(liste_vente, nb_requete_vente, nb_entrepots, a, graphe);
		}
		else if (strcmp(argv[3], "sans"))
		{
			for (int i = 0; i < nb_entrepots; i++)
			{
				libere_acteur(a[i]);
				free(graphe[i]);
			}
			free(a);
			free(graphe);

			printf("besoin d'arguments %s nom_fichier.csv type_algo(fin/insertion) type_echanges(sans/confiance/enchere) <option>nb_repetitions\n", argv[0]);
			return -1;
		}
		for (int i = 0; i < nb_entrepots; i++)
			a[i] = le_deficit_ou_pas(a[i], graphe);
	}
	else if (!strcmp(argv[2], "brute"))
	{
		FILE *f = fopen("resultat_brute", "w");
		if (f == NULL)
		{
			printf("Erreur d'ouverture du fichier\n");
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
		{
			float benefice = assignation_requete(a[i], graphe);
			fprintf(f, "acteur %d : %f\n", a[i].id_entrepot, benefice);
		}
		fclose(f);

		f = fopen("resultat_insertion_fin", "w");
		if (f == NULL)
		{
			printf("Erreur d'ouverture du fichier\n");
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
		{
			a[i] = evaluation_meilleure_solution(a[i].LR, a[i], a[i].nb_requete, graphe);
			a[i] = retour_a_la_casa(a[i], graphe);
			a[i] = le_deficit_ou_pas(a[i], graphe);
			fprintf(f, "acteur %d : %f\n", a[i].id_entrepot, (float)(a[i].benefice_total) / 10000);
		}
		fclose(f);

		struct entrepot *a2 = NULL;
		a2 = charge_entrepots(nomfic, graphe);

		f = fopen("resultat_insertion", "w");
		if (f == NULL)
		{
			printf("Erreur d'ouverture du fichier\n");
			return -1;
		}

		for (int i = 0; i < nb_entrepots; i++)
		{
			a2[i] = init_insertion(a2[i].LR, a2[i], a2[i].nb_requete, graphe);
			a2[i] = le_deficit_ou_pas(a2[i], graphe);
			fprintf(f, "acteur %d : %f\n", a2[i].id_entrepot, (float)(a2[i].benefice_total) / 10000);
		}
		fclose(f);

		for (int i = 0; i < nb_entrepots; i++)
		{
			libere_acteur(a[i]);
			libere_acteur(a2[i]);
			free(graphe[i]);
		}
		free(a);
		free(a2);
		free(graphe);

		return 0;
	}
	else
	{
		for (int i = 0; i < nb_entrepots; i++)
		{
			libere_acteur(a[i]);
			free(graphe[i]);
		}
		free(a);
		free(graphe);

		printf("Ce type d'algorithme n'existe pas, type_algo : fin, insertion ou brute\n");
		return -1;
	}

	analyse_donnees(a, nb_entrepots, type_enchere, grand_echantillon);
	if (!grand_echantillon)
		exporte_trajet(a, nb_entrepots);

	if (argc == 5 && nb_tours < atoi(argv[4]))
		goto repetition;

	for (int i = 0; i < nb_entrepots; i++)
	{
		libere_acteur(a[i]);
		free(graphe[i]);
	}
	free(a);
	free(graphe);

	return 0;
}