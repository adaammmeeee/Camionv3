#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include "structures.h"
#include "chemins_gloutons.h"

int faire_course(camion *c, int origine, int destination, int **graphe, int plein)
{
    if (origine == destination)
        return 0;

    if (c->taille_trajet == 0)
    {
        printf("Erreur probable dans l'initialisation des trajet, le trajet doit au moins contenir l'entrepot de départ\n");
        return -1;
    }
    else
    {
        c->trajet[c->taille_trajet] = destination;
        c->charge[c->taille_trajet - 1] = plein;
        c->taille_trajet++;
        c->distance_parcouru += graphe[origine][destination];
    }

    return cout_distance(graphe[origine][destination]);
}

// Renvoi la distance entre un camion cam et un sommet du graphe (originie_requete)
int proximite(int **graphe, camion cam, int origine_requete)
{
    int pos_camion = cam.trajet[cam.taille_trajet - 1];
    return graphe[pos_camion][origine_requete];
}

void fusion(int **graphe, int origine_requete, camion **liste_camion, int deb1, int fin1, int fin2)
{
    camion *liste_tmp;
    int deb2 = fin1 + 1;
    int compt1 = deb1;
    int compt2 = deb2;
    int i;

    liste_tmp = calloc((fin1 - deb1 + 1), sizeof(camion));

    for (i = deb1; i <= fin1; i++)
    {
        liste_tmp[i - deb1] = *liste_camion[i];
    }

    for (i = deb1; i <= fin2; i++)
    {
        if (compt1 == deb2)
            break;
        else if (compt2 == (fin2 + 1))
        {
            *liste_camion[i] = liste_tmp[compt1 - deb1];
            compt1++;
        }
        else if (proximite(graphe, liste_tmp[compt1 - deb1], origine_requete) < proximite(graphe, *liste_camion[compt2], origine_requete))
        {
            *liste_camion[i] = liste_tmp[compt1 - deb1];
            compt1++;
        }
        else
        {
            *liste_camion[i] = *liste_camion[compt2];
            compt2++;
        }
    }
    free(liste_tmp);
}

void tri_fusion_camion_proximite(int **graphe, int origine_requete, camion **liste_camion, int deb, int fin)
{
    if (deb < fin)
    {
        int milieu = (fin + deb) / 2;
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, deb, milieu);
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, milieu + 1, fin);
        fusion(graphe, origine_requete, liste_camion, deb, milieu, fin);
    }
}

int cout_requete_fin_trajet(requete *nouv, entrepot a, int *indice_camion, int **graphe)
{
    int distance = 0;
    tri_fusion_camion_proximite(graphe, nouv->origine, a.liste_camion, 0, a.nb_camion - 1);
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = a.liste_camion[i]->taille_trajet;
        int pos_camion = a.liste_camion[i]->trajet[taille_trajet - 1];
        int distance_parcouru = a.liste_camion[i]->distance_parcouru;

        if (distance_parcouru + graphe[pos_camion][nouv->origine] + graphe[nouv->origine][nouv->destination] + graphe[nouv->destination][a.id_entrepot] <= DISTANCE_MAX)
        {
            *indice_camion = i;
            distance = graphe[pos_camion][nouv->origine] + graphe[nouv->origine][nouv->destination] + graphe[nouv->destination][a.id_entrepot];
            break;
        }
    }
    return distance;
}

entrepot evaluation_meilleure_solution(liste_requete *LR, entrepot a, int nb_requete, int **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.benefice_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête à evaluer, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton
    requete *actuelle = LR->prem;
    while (actuelle && nb_requete)
    {
        int camion = -1;
        int distance = cout_requete_fin_trajet(actuelle, a, &camion, graphe);
        if (camion == -1 && distance)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if (distance)
        {
            int taille_trajet = a.liste_camion[camion]->taille_trajet;
            int pos_camion = a.liste_camion[camion]->trajet[taille_trajet - 1];
            faire_course(a.liste_camion[camion], pos_camion, actuelle->origine, graphe, 0);
            faire_course(a.liste_camion[camion], actuelle->origine, actuelle->destination, graphe, 1);
            actuelle->a_vendre = 0;
        }
        else{
            actuelle->a_vendre = 1;
        }

        actuelle = actuelle->suiv;
        nb_requete--;
    }

    if (nb_requete != 0 && actuelle == NULL)
    {
        printf("Attention la liste de requete contient moins de requete que le nombre indiqué en argument\n");
        return err;
    }
    return a;
}

int insertion(requete *r, entrepot a, int *id_camion, int *new_trajet, int *new_charge, int *taille_new_trajet, int amelioration, int **graphe)
{
    int meilleure_distance = INT_MAX;
    int distance_parcourue_min = DISTANCE_MAX;
    int position_insertion;

    for (int cpt_camion = 0; cpt_camion < a.nb_camion; cpt_camion++)
    {
        for(position_insertion = 0; position_insertion < a.liste_camion[cpt_camion]->taille_trajet; position_insertion++)
        {
            int distance_requete = 0;
            int taille_ajout = 0;
            int suite_trajet = position_insertion + 1;
            int trajet_vide = 1;

            int depart = a.liste_camion[cpt_camion]->trajet[position_insertion];
            int arrivee;
            // Si la requete n'est pas ajoutée à la fin du trajet et que le sommet position_insertion est l'origine d'un trajet vide
            // l'arrivée est le prochain sommet de la liste de trajet
            // sinon l'arrivee correspond au départ, un retour au point d'origine de la prochaine requête est nécessaire pour chercher la marchandise
            if ((position_insertion < a.liste_camion[cpt_camion]->taille_trajet - 1) && !a.liste_camion[cpt_camion]->charge[position_insertion])
                arrivee = a.liste_camion[cpt_camion]->trajet[position_insertion + 1];
            else
                arrivee = a.liste_camion[cpt_camion]->trajet[position_insertion];

            distance_requete = graphe[depart][r->origine] + graphe[r->origine][r->destination] + graphe[r->destination][arrivee] - graphe[depart][arrivee];
                  
            if(a.liste_camion[cpt_camion]->distance_parcouru + distance_requete <= DISTANCE_MAX)
            {
                int condition = amelioration?0:(distance_requete == meilleure_distance) && (a.liste_camion[cpt_camion]->distance_parcouru < distance_parcourue_min);
                if((distance_requete < meilleure_distance) || condition)
                {
                    for (int i = 0; i <= position_insertion; i++)
                        new_trajet[i] = a.liste_camion[cpt_camion]->trajet[i];

                    for (int i = 0; i < position_insertion; i++)
                        new_charge[i] = a.liste_camion[cpt_camion]->charge[i];

                    if (r->origine != depart)
                    {
                        new_trajet[suite_trajet] = r->origine;
                        new_charge[suite_trajet - 1] = 0;
                        new_charge[suite_trajet] = 1;
                        suite_trajet++;
                        taille_ajout++;
                    }
                    else
                        new_charge[suite_trajet - 1] = 1;

                    if (r->destination != arrivee)
                    {
                        new_trajet[suite_trajet] = r->destination;
                        new_charge[suite_trajet] = 0;
                        suite_trajet++;
                        taille_ajout++;
                    }
                    if (arrivee == depart)
                    {
                        trajet_vide = 0;
                        taille_ajout++;
                    }

                    for (int i = 0; i < (a.liste_camion[cpt_camion]->taille_trajet - (position_insertion + trajet_vide)); i++)
                        new_trajet[i + suite_trajet] = a.liste_camion[cpt_camion]->trajet[i + position_insertion + trajet_vide];

                    for (int i = 0; i < ((a.liste_camion[cpt_camion]->taille_trajet - 1) - (position_insertion + trajet_vide)); i++)
                        new_charge[i + suite_trajet] = a.liste_camion[cpt_camion]->charge[i + position_insertion + trajet_vide];

                    *taille_new_trajet = a.liste_camion[cpt_camion]->taille_trajet + taille_ajout;
                    *id_camion = cpt_camion;
                    meilleure_distance = distance_requete;
                    distance_parcourue_min = a.liste_camion[cpt_camion]->distance_parcouru;
                }
            }
        }
    }

    return meilleure_distance;
}

entrepot init_insertion(liste_requete *LR, entrepot a, int nb_requete, int **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.benefice_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête à evaluer, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton
    requete *actuelle = LR->prem;
    int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
    int *new_charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));

    while (actuelle && nb_requete)
    {
        int camion = -1;
        int taille_new_trajet = 0;
        int distance = insertion(actuelle, a, &camion, new_trajet, new_charge, &taille_new_trajet, 0, graphe);

        if ((camion == -1 || !taille_new_trajet) && distance != INT_MAX)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if (distance < INT_MAX)
        {
            for (int i = 0; i < taille_new_trajet; i++)
                a.liste_camion[camion]->trajet[i] = new_trajet[i];

            for (int i = 0; i < taille_new_trajet - 1; i++)
                a.liste_camion[camion]->charge[i] = new_charge[i];

            a.liste_camion[camion]->taille_trajet = taille_new_trajet;

            a.liste_camion[camion]->distance_parcouru += distance;
            actuelle->a_vendre = 0;
        }
        else if(distance == INT_MAX)
            actuelle->a_vendre = 1;

        actuelle = actuelle->suiv;
        nb_requete--;
    }

    if (nb_requete != 0 && actuelle == NULL)
    {
        printf("Attention la liste de requete contient moins de requete que le nombre indiqué en argument\n");
        return err;
    }
    free(new_trajet);
    free(new_charge);

    return a;
}
