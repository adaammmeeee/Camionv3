#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "brute_force.h"

int complexite = 0;
int nb_combinaison = 0;

// Affiche un tableau d'une certaine taille
void affiche_tableau(int *tableau, int taille)
{
    for (int i = 0; i < taille; i++)
    {
        (tableau[i] >= 0) ? (printf("%d ", tableau[i])) : (0);
    }
    printf("\n");
}

// soit tab un tableau contenant des indices de requetes, on calcule le benefice de toutes ces requêtes
// elles sont effectués par un seul camion qui part de son entrepot ID
int calcul_cout_tab_requete(int *tab, int taille_tab, requete *r, entrepot a, int **graphe)
{
    int benefice = 0;
    int kilometrage = 0;
    int gain = 0;
    int cout_trajet_total = 0;
    int cout_trajet = 0;
    int pos_camion = a.id_entrepot;
    int pos_avant = a.id_entrepot;
    if (tab[0] == -1)
    {
        // printf("Pas de trajet possible\n");
    }
    // printf("Itinéraire calculé : %d -> ", pos_camion);
    for (int i = 0; i < taille_tab; i++)
    {
        cout_trajet = 0;
        if (tab[i] < 0)
            break;
        int indice_requete = tab[i];
        if (pos_camion != r[indice_requete].origine)
        {
            kilometrage += graphe[pos_camion][r[indice_requete].origine];
            cout_trajet += cout_distance(graphe[pos_camion][r[indice_requete].origine]);
            pos_camion = r[indice_requete].origine;
            //  printf("-v-> %d", pos_camion);
        }

        kilometrage += graphe[pos_camion][r[indice_requete].destination];
        cout_trajet += cout_distance(graphe[pos_camion][r[indice_requete].destination]);
        pos_camion = r[indice_requete].destination;
        if (kilometrage + graphe[pos_camion][a.id_entrepot] <= DISTANCE_MAX)
        {
            gain += r[indice_requete].gain;
            cout_trajet_total += cout_trajet;
            pos_avant = pos_camion; // On valide la position du camion
        }

        else
        {
            gain -= r[indice_requete].perte;
            pos_camion = pos_avant;
        }

        // printf("%d -> ", pos_camion);
    }
    // On rentre à l'entrepot
    kilometrage += graphe[pos_camion][a.id_entrepot];
    cout_trajet_total += cout_distance(graphe[pos_camion][a.id_entrepot]);
    // printf("%d\n", a.id_entrepot);
    benefice = gain - cout_trajet_total;

    // printf("Le tableau : ");
    // affiche_tableau(tab, taille_tab);
    // printf("Benefice: %.2f\n", (float)benefice / 10000);
    //  printf("\n\n");
    return benefice;
}

// Incremente le tableau, en base limite
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

// Calcul tous les ordres possibles d'un tableau tab, et renvoi l'ordre de requête avec le meilleur benefice dans best_tab
int different_ordre(int *tab, int taille_tab, int *new_tab, int *best_tab, int *case_noir, int cpt, int *meilleur_cout, requete *r, entrepot a, int **graphe)
{
    // On a un ordre
    if (cpt == taille_tab)
    {
        complexite++;
        new_tab[0] = tab[0];
        int nouveau_cout = calcul_cout_tab_requete(new_tab, taille_tab, r, a, graphe);
        // affiche_tableau(new_tab, taille_tab);
        // affiche_tableau(best_tab, taille_tab);

        if (nouveau_cout > *meilleur_cout)
        {
            memcpy(best_tab, new_tab, sizeof(int) * taille_tab);
            *meilleur_cout = nouveau_cout;
            // printf("best est devenu : ");
            // affiche_tableau(best_tab, taille_tab);
            // printf("best (cout %f$)  :  ", (float)*meilleur_cout/10000);
        }

        return 0;
    }

    for (int i = 1; i < taille_tab; i++)
    {
        if (case_noir[i] == 0)
        {
            new_tab[cpt] = tab[i];
            case_noir[i] = 1;
            different_ordre(tab, taille_tab, new_tab, best_tab, case_noir, cpt + 1, meilleur_cout, r, a, graphe);
            case_noir[i] = 0;
        }
    }
    return 0;
}

// rempli tab_a_remplir avec touts les entiers qui ne sont pas présent dans cmp1 ET cmp2, renvoi la taille de ce tableau
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

int int_pow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp % 2)
            result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}


int combinaison(int *tab, int *tab_ref, int n, int k, int index, int cpt, int **camion_requete, int **meilleure_assignation, int *cout_meilleure_assignation, requete *r, entrepot a, int **graphe)
{
    // On a trouvé une combinaison !
    if (index == k)
    {
        nb_combinaison++;
        // Camion requete est un tableau à double entrée [i][k], i désigne le numéro de camion et k son trajet
        // On initialise le premier sommet de chaque camion afin de tous les rendre unique, d'après la combinaison calculer dans k
        // printf("\n\n\n\n");
        for (int i = 0; i < k; i++)
        {
            camion_requete[i][0] = tab[i];
        }
        int pos_curseur[k];
        int requete_restante[n];
        int nb_requete_restante = 0;
        // Ici on charge toutes les requêtes qui ne sont pas utilisés dans requete_restante: on va les attribuer de manière différentes à tous les camion et recup la meilleure combinaison
        nb_requete_restante = rempli_difference(requete_restante, tab_ref, n, tab, k);
        // les indices d'assignation représente le numéro de la requete
        // La valeur des cases d'assignation représente le numéro de camion qui prend la requête
        int *assignation = calloc(sizeof(int), nb_requete_restante);
        int nb_incrementation = int_pow(k, nb_requete_restante);
        // printf("Nombre d'incrementation : %d\n", nb_incrementation);
        int organisation_actuelle = 0;

        for (int z = 0; z < nb_incrementation; z++)
        {

            //  On passe par un tableau de position de curseur, pour pouvoir concaténer à chaque fois la requête à assigner bien à la fin sans parcourir en boucle le tableau
            //  pos_curseur[k] désigne la taille actuelle du trajet du camion k
            for (int i = 0; i < k; i++)
            {
                pos_curseur[i] = 1;
                memset(camion_requete[i] + 1, 0, sizeof(int) * (n - 1));
                // affiche_tableau(camion_requete[i], pos_curseur[i]);
            }

            for (int j = 0; j < nb_requete_restante; j++)
            {
                int indice_camion = assignation[j];
                camion_requete[indice_camion][pos_curseur[indice_camion]] = requete_restante[j];
                pos_curseur[indice_camion]++;
            }

            // Pour tous les camions
            for (int i = 0; i < k; i++)
            {
                int new_tab[pos_curseur[i]];
                // Case noir est utilise pour connaître les différents ordres
                int case_noir[pos_curseur[i]];
                for (int j = 0; j < pos_curseur[i]; j++)
                {
                    case_noir[j] = 0;
                }

                // Best tab sera l'ordre qui nous rapporte le plus
                int best_tab[pos_curseur[i]];
                memset(best_tab, 0, sizeof(int) * pos_curseur[i]);

                // printf("On va récupérer le meilleur ordre de requete pour : \n");
                // affiche_tableau(camion_requete[i], pos_curseur[i]);
                // printf("Les ordres possibles (hors premier élément) sont : \n");
                int meilleur_cout = INT_MIN + 1;

                different_ordre(camion_requete[i], pos_curseur[i], new_tab, best_tab, case_noir, 1, &meilleur_cout, r, a, graphe);
                // printf("On a récupérer le meilleur cout %f$ \n",(float) (meilleur_cout/10000));
                organisation_actuelle += meilleur_cout;
                memcpy(camion_requete[i], best_tab, sizeof(int) * pos_curseur[i]);
                // printf("Le meilleur ordre nous a rapporté : %f$, le voici : \n", meilleur_cout);
                // affiche_tableau(camion_requete[i], pos_curseur[i]);
            }

            if (organisation_actuelle > *cout_meilleure_assignation)
            {

                *cout_meilleure_assignation = organisation_actuelle;
                for (int i = 0; i < k; i++)
                {
                    memset(meilleure_assignation[i], -1, sizeof(int) * n);
                    memcpy(meilleure_assignation[i], camion_requete[i], sizeof(int) * pos_curseur[i]);
                    // affiche_tableau(meilleure_assignation[i], n);
                }
            }
            organisation_actuelle = 0;
            // printf("\n\n\n\n");
            if ((z + 1) < nb_incrementation)
            {
                incremente_tableau(assignation, nb_requete_restante, k - 1);
            }
        }

        free(assignation);

        return 0;
    }

    // On cherche les combinaisons
    for (int i = cpt; i < n && index < k; i++)
    {
        tab[index] = tab_ref[i];
        combinaison(tab, tab_ref, n, k, index + 1, i + 1, camion_requete, meilleure_assignation, cout_meilleure_assignation, r, a, graphe);
    }
    return 0;
}

// brut force
float assignation_requete(entrepot a, int **graphe)
{
    printf("\n\n\n\n");
    printf("Entrepot : %d\n\n", a.id_entrepot);
    // chaque indice du tab_requete représente le numéro de requête, la valeur dans cette case représente le numéro de camion qui traite la demande

    requete r[a.nb_requete];
    requete *copy = a.LR->prem;
    for (int i = 0; i < a.nb_requete; i++)
    {
        r[i].origine = copy->origine;
        r[i].destination = copy->destination;
        r[i].gain = copy->gain;
        r[i].perte = copy->perte;
        copy = copy->suiv;
    }
    // On a chargé notre tableau de requete pour éviter de parcourir la liste chaînée en boucle

    // Maintenant pour chaque camion, on va assigner les requêtes à traiter
    int n = a.nb_requete; // nombre de requete
    // int k = a.nb_camion; // nombre de camion
    float meilleur_benefice = INT_MIN + 1;
    int meilleur_k = 0;

    for (int k = 1; (k <= a.nb_camion) && (k < n); k++)
    {
        printf("On va assigner les requêtes à %d camions\n", k);
        int **camion_requete;
        camion_requete = calloc(k, sizeof(int *));
        for (int i = 0; i < k; i++)
        {
            camion_requete[i] = calloc(n, sizeof(int));
        }

        int **meilleure_assignation = calloc(k, sizeof(int *));
        for (int i = 0; i < k; i++)
            meilleure_assignation[i] = calloc(n, sizeof(int));

        // printf("\n\nNombre de requete : %d\n", n);
        // printf("Nombre de camion  : %d\n", k);

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

        int cout_meilleure_assignation = INT_MIN;
        float benefice_total = 0;
        combinaison(new_tab, tab_ref, n, k, 0, 0, camion_requete, meilleure_assignation, &cout_meilleure_assignation, r, a, graphe);
        // printf("Voici la meilleure assignation possible des requêtes pour %d camions :  \n", k);
        for (int i = 0; i < k; i++)
        {
            printf("requête du camion %d: ", i);
            affiche_tableau(meilleure_assignation[i], n);
            float buffer = (float)calcul_cout_tab_requete(meilleure_assignation[i], n, r, a, graphe) / 10000;
            printf("Benefice : %f$\n", buffer);
            benefice_total += buffer;
        }
        // printf("\nBenefice acteur %d : %f$\n", a.id_entrepot, benefice_total);
        if (benefice_total > meilleur_benefice)
        {
            meilleur_benefice = benefice_total;
            meilleur_k = k;
        }

        for (int i = 0; i < k; i++)
        {
            free(meilleure_assignation[i]);
            free(camion_requete[i]);
        }
        free(camion_requete);
        free(meilleure_assignation);
    }
    // printf("nombre de combinaison : %d\n", nb_combinaison);
    // printf("complexite : %d\n", complexite);
    printf("meilleur k : %d\n", meilleur_k);
    return meilleur_benefice;
}