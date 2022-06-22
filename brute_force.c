#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "brute_force.h"

void affiche_tableau(int *tableau, int taille)
{
    for (int i = 0; i < taille; i++)
    {
        printf("%d ", tableau[i]);
    }
    printf("\n");
}

void incremente_tableau(int *tableau, int taille, int limite)
{
    if (tableau[taille - 1] == limite)
    {
        int cpt = 1;
        while (tableau[taille - cpt] == limite)
        {
            tableau[taille - cpt] = 0;
            if (taille - cpt > 0)
                cpt++;
        }
        if (taille - cpt >= 0)
            tableau[taille - cpt]++;
    }
    else
    {
        tableau[taille - 1]++;
    }
}

float calcul_cout_tab_requete(int *tab, int taille_tab, requete *tab_requete, entrepot a, float **graphe)
{
    float gain_total = 0;
    int pos_camion = a.id_entrepot;
    if (tab[0] == -1)
        return -10000000;
    // printf("Itinéraire calculé : %d -> ", pos_camion);
    for (int i = 0; i < taille_tab; i++)
    {
        int indice_requete = tab[i];
        gain_total += tab_requete[indice_requete].gain;

        gain_total -= cout_distance(graphe[pos_camion][tab_requete[indice_requete].origine]);
        pos_camion = tab_requete[indice_requete].origine;
        // printf("%d -> ", pos_camion);

        gain_total -= cout_distance(graphe[tab_requete[indice_requete].origine][tab_requete[indice_requete].destination]);
        pos_camion = tab_requete[indice_requete].destination;
        // printf("%d -> ", pos_camion);
    }

    gain_total -= cout_distance(graphe[pos_camion][a.id_entrepot]);
    // printf("%d\n", a.id_entrepot);

    return gain_total;
}

float different_ordre(int *tab, int taille_tab, int *new_tab, int *best_tab, int *case_noir, int cpt, requete *r, entrepot a, float **graphe)
{
    if (cpt == taille_tab)
    {
        new_tab[0] = tab[0];

        float nouveau_cout = calcul_cout_tab_requete(new_tab, taille_tab, r, a, graphe);
        float meilleur_cout = calcul_cout_tab_requete(best_tab, taille_tab, r, a, graphe);
        if (nouveau_cout >= meilleur_cout)
        {
            // printf("Meilleur ordre trouvé ! \n");
            memcpy(best_tab, new_tab, sizeof(int) * taille_tab);
            // affiche_tableau(best_tab, taille_tab);
            meilleur_cout = nouveau_cout;
        }

        return meilleur_cout;
    }

    for (int i = 1; i < taille_tab; i++)
    {
        if (case_noir[i] == 0)
        {
            new_tab[cpt] = tab[i];
            case_noir[i] = 1;
            return different_ordre(tab, taille_tab, new_tab, best_tab, case_noir, cpt + 1, r, a, graphe);
            case_noir[i] = 0;
        }
    }
}

// rempli tab_a_remplir avec touts les entiers qui ne sont pas présent dans cmp1 ET cmp2 à la fois, renvoi la taille de ce tableau
int rempli_difference(int *tab_a_remplir, int *cmp1, int taille_cmp1, int *cmp2, int taille_cmp2)
{
    int cpt = 0;
    for (int i = 0; i < taille_cmp1; i++)
    {
        int trouve = 0;
        for (int j = 0; j < taille_cmp2; j++)
        {
            if (cmp1[i] == cmp2[j])
            {
                trouve = 1;
                break;
            }
        }
        if (!trouve)
        {
            tab_a_remplir[cpt] = cmp1[i];
            cpt++;
        }
    }
    return cpt;
}

float combinaison(int *tab, int *tab_ref, int n, int k, int index, int cpt, int **camion_requete, requete *r, entrepot a, float **graphe)
{
    // On a trouvé une combinaison !

    if (index == k)
    {
        // Camion requete est un tableau à double entrée [i][k], i désigne le numéro de camion et k son trajet
        // On initialise le premier sommet de chaque camion afin de tous les rendre unique, d'après la combinaison calculer dans k
        for (int i = 0; i < k; i++)
            camion_requete[i][0] = tab[i];

        int pos_curseur[k];

        int requete_restante[n];
        int nb_requete_restante = 0;
        // Ici on charge toutes les requêtes qui ne sont pas utilisés dans requete_restante: on va les attribuer de manière différentes à tous les camion et recup la meilleure combinaison
        nb_requete_restante = rempli_difference(requete_restante, tab_ref, n, tab, k);

        // les indices d'assignation représente le numéro de la requete
        // La valeur des cases d'assignation représente le numéro de camion qui prend la requête
        int *assignation = calloc(sizeof(int), nb_requete_restante);
        int nb_incrementation = k;
        for (int i = 0; i < nb_requete_restante - 1; i++) // On va incrémenteer k^(nb_requete_restante-1) fois pour obtenir toutes les combinaisons possibles
            nb_incrementation *= nb_incrementation;       // Simplement utiliser la blibothèque math.h ?

        float total = 0;
        for (int z = 0; z < nb_incrementation; z++)
        {
            // On passe par un tableau de position de curseur, pour pouvoir concaténer à chaque fois la requête à assigner bien à la fin sans parcourir en boucle le tableau
            // pos_curseur[k] désigne la taille actuelle du trajet du camion k
            for (int i = 0; i < k; i++)
            {
                pos_curseur[i] = 1;
                memset(camion_requete[i] + 1, 0, sizeof(int) * (n - 1));
            }

            for (int j = 0; j < nb_requete_restante; j++)
            {
                int indice_camion = assignation[j];
                camion_requete[indice_camion][pos_curseur[indice_camion]] = requete_restante[j];
                pos_curseur[indice_camion]++;
            }
            for (int i = 0; i < k; i++)
            {
                int new_tab[pos_curseur[i]];
                int case_noir[pos_curseur[i]];
                for (int j = 0; j < pos_curseur[i]; j++)
                {
                    case_noir[j] = 0;
                }
                int best_tab[pos_curseur[i]];
                memset(best_tab, -1, sizeof(int) * pos_curseur[i]);
                total += different_ordre(camion_requete[i], pos_curseur[i], new_tab, best_tab, case_noir, 1, r, a, graphe);
                memcpy(camion_requete[i], best_tab, sizeof(int) * pos_curseur[i]);
                //affiche_tableau(best_tab, pos_curseur[i]);
            }
            //printf("Total de cette initialisation : %.2f\n", total);
            total = 0;

            incremente_tableau(assignation, nb_requete_restante, k - 1);
        }

        free(assignation);

        return 0;
    }

    for (int i = cpt; i < n && index < k; i++)
    {
        tab[index] = tab_ref[i];
        combinaison(tab, tab_ref, n, k, index + 1, i + 1, camion_requete, r, a, graphe);
    }
    return 0;
}

// brut force
int assignation_requete(entrepot a, float **graphe)
{

    // chaque indice du tab_requete représente le numéro de requête, la valeur dans cette case représente le numéro de camion qui traite la demande

    requete r[a.nb_requete];
    requete *copy = a.LR->prem;
    for (int i = 0; i < a.nb_requete; i++)
    {
        r[i].origine = copy->origine;
        r[i].destination = copy->destination;
        r[i].gain = copy->gain;
        copy = copy->suiv;
    }
    // On a chargé notre tableau de requete pour éviter de parcourir la liste chaînée en boucle

    // Maintenant pour chaque camion, on va assigner les requêtes à traiter
    int **camion_requete;
    camion_requete = calloc(a.nb_camion, sizeof(int *));
    for (int i = 0; i < a.nb_camion; i++)
    {
        camion_requete[i] = calloc(a.nb_requete, sizeof(int));
    }

    int n = a.nb_requete;
    int k = a.nb_camion;

    printf("Nombre de requete : %d\n", n);
    printf("Nombre de camion  : %d\n", k);

    int new_tab[k];
    for (int i = 0; i < k - 1; i++)
    {
        new_tab[i] = 0;
    }

    int tab_ref[n];
    for (int i = 0; i < n; i++)
    {
        tab_ref[i] = i;
    }

    combinaison(new_tab, tab_ref, n, k, 0, 0, camion_requete, r, a, graphe);

    for (int i = 0; i < a.nb_camion; i++)
    {
        free(camion_requete[i]);
    }
    free(camion_requete);
    return 0;
}
