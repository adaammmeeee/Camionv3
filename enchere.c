#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "structures.h"
#include "enchere.h"

// renvoi un tableau de taille k comprenant toutes les valeurs de 0 à k, dans le desordre
int *tab_melange(int k)
{

    int val[k];
    for (int i = 0; i < k; i++)
    {
        val[i] = i;
    }
    int *tab = malloc(k * sizeof(int));
    int r;
    for (int i = 0; i < k; i++)
    {
        r = rand() % (k - i);
        tab[i] = val[r];
        val[r] = val[k - i - 1];
    }
    return tab;
}

int marge(entrepot a)
{
    int moy_prix = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        if (a.liste_camion[i]->nb_requetes_faites)
            a.liste_camion[i]->benefice_camion_moy /= a.liste_camion[i]->nb_requetes_faites;

        moy_prix += a.liste_camion[i]->benefice_camion_moy;
    }
    moy_prix /= a.nb_camion;

    if (moy_prix < 0)
        moy_prix = 0;

    return moy_prix;
}

int nb_vente(entrepot *a, int nb_entrepot)
{
    int nb_requete = 0;
    for (int i = 0; i < nb_entrepot; i++)
    {
        requete *actuelle = a[i].LR->prem;
        while (actuelle)
        {
            if (actuelle->a_vendre)
            {
                nb_requete++;
            }
            actuelle = actuelle->suiv;
        }
    }

    return nb_requete;
}

requete **mise_en_vente(entrepot *a, int nb_entrepot, int *nb_requete_vente)
{
    *nb_requete_vente = nb_vente(a, nb_entrepot);
    if (!(*nb_requete_vente))
        return NULL;

    requete **liste_vente = calloc(*nb_requete_vente, sizeof(requete *));

    int cpt_vente = 0;
    for (int i = 0; i < nb_entrepot; i++)
    {
        requete *actuelle = a[i].LR->prem;
        while (actuelle)
        {
            if (actuelle->a_vendre)
            {
                actuelle->prix_propose_vente = actuelle->gain;
                liste_vente[cpt_vente] = actuelle;
                cpt_vente++;
            }
            actuelle = actuelle->suiv;
        }
    }

    // Gestion d'erreur
    if (cpt_vente != *nb_requete_vente)
        return NULL;

    return liste_vente;
}

entrepot *enchere_echange_fin(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{
    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete]->id_entrepot;
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        int cout_requete_min = rv[cpt_requete]->prix_propose_vente;

        int *tab = tab_melange(nb_entrepot);
        for (int i = 0; i < nb_entrepot; i++)
        {
            int cpt_entrepot = tab[i];
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_offre], &camion_offre, graphe);
                if (camion_offre == -1 && cout_requete)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }
                if (cout_requete && cout_requete < cout_requete_min)
                {
                    cout_requete += +marge(a[indice_e_offre]);
                    cout_requete_min = cout_requete;
                    indice_e_offre_min = indice_e_offre;
                    indice_c_offre_min = camion_offre;
                    cpt_offre++;
                }
            }
        }
        free(tab);

        if (cpt_offre)
        {
            int taille_trajet = a[indice_e_offre_min].liste_camion[indice_c_offre_min]->taille_trajet;
            int pos_camion = a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet[taille_trajet - 1];

            faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], pos_camion, rv[cpt_requete]->origine, graphe, 0);
            faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], rv[cpt_requete]->origine, rv[cpt_requete]->destination, graphe, 1);
            a[indice_e_offre_min].benefice_total += cout_requete_min;
            a[indice_e_demande].benefice_total -= cout_requete_min;
            rv[cpt_requete]->a_vendre = 0;

            // printf("ENCHERES : La requete %d->%d de l'acteur %d a été vendue à l'acteur %d au prix de %d, il la fera avec le camion %d\n",
            //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_offre_min].id_entrepot, cout_requete_min + 1, a[indice_e_offre_min].liste_camion[indice_c_offre_min]->id_camion);
        }
        else
        {
            int camion_demande = -1;
            int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_demande], &camion_demande, graphe);
            if (camion_demande == -1 && cout_requete)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if (camion_demande != -1 && cout_requete)
            {
                int indice_c_demande = camion_demande;
                int taille_trajet = a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet;
                int pos_camion = a[indice_e_demande].liste_camion[indice_c_demande]->trajet[taille_trajet - 1];

                faire_course(a[indice_e_demande].liste_camion[indice_c_demande], pos_camion, rv[cpt_requete]->origine, graphe, 0);
                faire_course(a[indice_e_demande].liste_camion[indice_c_demande], rv[cpt_requete]->origine, rv[cpt_requete]->destination, graphe, 1);
                rv[cpt_requete]->a_vendre = 0;

                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il la fera avec le camion %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_demande].liste_camion[indice_c_demande]->id_camion);
            }
            else if (!cout_requete)
            {
                rv[cpt_requete]->a_vendre = 1;

                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il ne peut pas la faire, il perdra %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, rv[cpt_requete].perte);
            }
        }
    }
    free(rv);

    return a;
}

entrepot *enchere_echange_insertion(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{
    int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
    int *new_charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));
    int taille_new_trajet_min = 0;
    int *new_trajet_min = calloc(TAILLE_MAX_TRAJET, sizeof(int));
    int *new_charge_min = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));

    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete]->id_entrepot;
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        int cout_requete_min = rv[cpt_requete]->prix_propose_vente;
        int distance_requete_min = 0;

        int *tab = tab_melange(nb_entrepot);
        for (int i = 0; i < nb_entrepot; i++)
        {
            int cpt_entrepot = tab[i];
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                int taille_new_trajet = 0;
                int distance_requete = insertion(rv[cpt_requete], a[indice_e_offre], &camion_offre, new_trajet, new_charge, &taille_new_trajet, 0, graphe);
                int cout_requete = cout_distance(distance_requete) + marge(a[indice_e_offre]);

                if ((camion_offre == -1 || !taille_new_trajet) && distance_requete != INT_MAX)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }

                if (distance_requete != INT_MAX && cout_requete < cout_requete_min)
                {
                    distance_requete_min = distance_requete;
                    cout_requete_min = cout_requete;
                    indice_e_offre_min = indice_e_offre;
                    indice_c_offre_min = camion_offre;
                    for (int i = 0; i < taille_new_trajet_min; i++)
                        new_trajet_min[i] = new_trajet[i];

                    for (int i = 0; i < taille_new_trajet_min; i++)
                        new_charge_min[i] = new_charge[i];
                    taille_new_trajet_min = taille_new_trajet;
                    cpt_offre++;
                }
            }
        }
        free(tab);

        if (cpt_offre)
        {
            for (int i = 0; i < taille_new_trajet_min; i++)
                a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet[i] = new_trajet_min[i];

            for (int i = 0; i < taille_new_trajet_min; i++)
                a[indice_e_offre_min].liste_camion[indice_c_offre_min]->charge[i] = new_charge_min[i];

            a[indice_e_offre_min].liste_camion[indice_c_offre_min]->taille_trajet = taille_new_trajet_min;
            a[indice_e_offre_min].liste_camion[indice_c_offre_min]->distance_parcouru += distance_requete_min;

            a[indice_e_offre_min].benefice_total += cout_requete_min;
            a[indice_e_demande].benefice_total -= cout_requete_min;
            rv[cpt_requete]->a_vendre = 0;

            // printf("ENCHERES : La requete %d->%d de l'acteur %d a été vendue à l'acteur %d au prix de %d, il la fera avec le camion %d\n",
            //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_offre_min].id_entrepot, cout_requete_min + 1, a[indice_e_offre_min].liste_camion[indice_c_offre_min]->id_camion);
        }
        else
        {
            int camion_demande = -1;
            int taille_new_trajet = 0;
            int distance_requete = insertion(rv[cpt_requete], a[indice_e_demande], &camion_demande, new_trajet, new_charge, &taille_new_trajet, 0, graphe);

            if (camion_demande == -1 && distance_requete != INT_MAX)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if (camion_demande != -1 && distance_requete != INT_MAX)
            {
                int indice_c_demande = camion_demande;
                for (int i = 0; i < taille_new_trajet; i++)
                    a[indice_e_demande].liste_camion[indice_c_demande]->trajet[i] = new_trajet[i];

                for (int i = 0; i < taille_new_trajet - 1; i++)
                    a[indice_e_demande].liste_camion[indice_c_demande]->charge[i] = new_charge[i];

                a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet = taille_new_trajet;
                a[indice_e_demande].liste_camion[indice_c_demande]->distance_parcouru += distance_requete;
                rv[cpt_requete]->a_vendre = 0;

                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il la fera avec le camion %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_demande].liste_camion[indice_c_demande]->id_camion);
            }
            else if (distance_requete == INT_MAX)
            {
                rv[cpt_requete]->a_vendre = 1;
                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il ne peut pas la faire, il perdra %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, rv[cpt_requete].perte);
            }
        }
    }
    free(new_trajet);
    free(new_charge);
    free(new_trajet_min);
    free(new_charge_min);
    free(rv);

    return a;
}

entrepot *confiance_insertion(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{
    int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
    int *new_charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));

    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete]->id_entrepot;
        int cout_requete_demande = rv[cpt_requete]->prix_propose_vente;
        int offre = 0;

        int *tab = tab_melange(nb_entrepot);
        for (int i = 0; i < nb_entrepot; i++)
        {
            int cpt_entrepot = tab[i];
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                int taille_new_trajet = 0;
                int distance_requete = insertion(rv[cpt_requete], a[indice_e_offre], &camion_offre, new_trajet, new_charge, &taille_new_trajet, 0, graphe);
                int cout_requete = cout_distance(distance_requete);

                if ((camion_offre == -1 || !taille_new_trajet) && distance_requete != INT_MAX)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }

                if (distance_requete != INT_MAX && cout_requete < cout_requete_demande)
                {
                    for (int i = 0; i < taille_new_trajet; i++)
                        a[indice_e_offre].liste_camion[camion_offre]->trajet[i] = new_trajet[i];

                    for (int i = 0; i < taille_new_trajet; i++)
                        a[indice_e_offre].liste_camion[camion_offre]->charge[i] = new_charge[i];

                    a[indice_e_offre].liste_camion[camion_offre]->taille_trajet = taille_new_trajet;
                    a[indice_e_offre].liste_camion[camion_offre]->distance_parcouru += distance_requete;

                    a[indice_e_offre].benefice_total += cout_requete_demande;
                    a[indice_e_demande].benefice_total -= cout_requete_demande;

                    rv[cpt_requete]->a_vendre = 0;

                    offre = 1;
                    break;
                }
            }
        }
        free(tab);
        if (!offre)
        {
            int camion_demande = -1;
            int taille_new_trajet = 0;
            int distance_requete = insertion(rv[cpt_requete], a[indice_e_demande], &camion_demande, new_trajet, new_charge, &taille_new_trajet, 0, graphe);

            if (camion_demande == -1 && distance_requete != INT_MAX)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if (camion_demande != -1 && distance_requete != INT_MAX)
            {
                int indice_c_demande = camion_demande;
                for (int i = 0; i < taille_new_trajet; i++)
                    a[indice_e_demande].liste_camion[indice_c_demande]->trajet[i] = new_trajet[i];

                for (int i = 0; i < taille_new_trajet - 1; i++)
                    a[indice_e_demande].liste_camion[indice_c_demande]->charge[i] = new_charge[i];

                a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet = taille_new_trajet;
                a[indice_e_demande].liste_camion[indice_c_demande]->distance_parcouru += distance_requete;
                rv[cpt_requete]->a_vendre = 0;

                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il la fera avec le camion %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_demande].liste_camion[indice_c_demande]->id_camion);
            }
            else if (distance_requete == INT_MAX)
            {
                rv[cpt_requete]->a_vendre = 1;
                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il ne peut pas la faire, il perdra %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, rv[cpt_requete].perte);
            }
        }
    }
    free(new_trajet);
    free(new_charge);
    free(rv);

    return a;
}

entrepot *confiance_fin(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{
    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete]->id_entrepot;
        int offre = 0;
        int cout_requete_demande = rv[cpt_requete]->prix_propose_vente;

        int *tab = tab_melange(nb_entrepot);
        for (int i = 0; i < nb_entrepot; i++)
        {
            int cpt_entrepot = tab[i];
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_offre], &camion_offre, graphe);
                if (camion_offre == -1 && cout_requete)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }

                if (cout_requete && cout_requete < cout_requete_demande)
                {
                    int taille_trajet = a[indice_e_offre].liste_camion[camion_offre]->taille_trajet;
                    int pos_camion = a[indice_e_offre].liste_camion[camion_offre]->trajet[taille_trajet - 1];

                    faire_course(a[indice_e_offre].liste_camion[camion_offre], pos_camion, rv[cpt_requete]->origine, graphe, 0);
                    faire_course(a[indice_e_offre].liste_camion[camion_offre], rv[cpt_requete]->origine, rv[cpt_requete]->destination, graphe, 1);
                    a[indice_e_offre].benefice_total += cout_requete_demande;
                    a[indice_e_demande].benefice_total -= cout_requete_demande;
                    rv[cpt_requete]->a_vendre = 0;

                    offre = 1;
                    break;
                }
            }
        }
        free(tab);

        if (!offre)
        {
            int camion_demande = -1;
            int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_demande], &camion_demande, graphe);
            if (camion_demande == -1 && cout_requete)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if (camion_demande != -1 && cout_requete)
            {
                int indice_c_demande = camion_demande;
                int taille_trajet = a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet;
                int pos_camion = a[indice_e_demande].liste_camion[indice_c_demande]->trajet[taille_trajet - 1];

                faire_course(a[indice_e_demande].liste_camion[indice_c_demande], pos_camion, rv[cpt_requete]->origine, graphe, 0);
                faire_course(a[indice_e_demande].liste_camion[indice_c_demande], rv[cpt_requete]->origine, rv[cpt_requete]->destination, graphe, 1);
                rv[cpt_requete]->a_vendre = 0;

                // printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il la fera avec le camion %d\n",
                //         rv[cpt_requete].origine, rv[cpt_requete].destination, a[indice_e_demande].id_entrepot, a[indice_e_demande].liste_camion[indice_c_demande]->id_camion);
            }
        }
    }
    free(rv);

    return a;
}