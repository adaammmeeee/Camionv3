#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "chemins_gloutons.h"

float faire_course(camion *c, int origine, int destination, float **graphe, int plein)
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
    }

    c->distance_parcouru += graphe[origine][destination];
    return cout_distance(graphe[origine][destination]);
}

// Renvoi la distance entre un camion cam et un sommet du graphe (originie_requete)
float proximite(float **graphe, camion cam, int origine_requete)
{
    int pos_camion = cam.trajet[cam.taille_trajet - 1];
    return graphe[pos_camion][origine_requete];
}

void fusion(float **graphe, int origine_requete, camion **liste_camion, int deb1, int fin1, int fin2)
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

void tri_fusion_camion_proximite(float **graphe, int origine_requete, camion **liste_camion, int deb, int fin)
{
    if (deb < fin)
    {
        int milieu = (fin + deb) / 2;
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, deb, milieu);
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, milieu + 1, fin);
        fusion(graphe, origine_requete, liste_camion, deb, milieu, fin);
    }
}

int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, float **graphe)
{
    float cout = 0;
    tri_fusion_camion_proximite(graphe, nouv.origine, a.liste_camion, 0, a.nb_camion - 1);
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = a.liste_camion[i]->taille_trajet;
        int pos_camion = a.liste_camion[i]->trajet[taille_trajet - 1];
        float distance_parcouru = a.liste_camion[i]->distance_parcouru;
        *indice_camion = i;

        if (distance_parcouru + graphe[pos_camion][nouv.origine] + graphe[nouv.origine][nouv.destination] + graphe[nouv.destination][a.id_entrepot] <= DISTANCE_MAX)
        {
            cout += cout_distance(graphe[pos_camion][nouv.origine]);
            cout += cout_distance(graphe[nouv.origine][nouv.destination]);
            cout += cout_distance(graphe[nouv.destination][a.id_entrepot]);
            break;
        }
    }
    return cout;
}

entrepot evaluation_meilleure_solution(liste_requete *LR, entrepot a, int nb_requete, float **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.gain_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête à evaluer, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton
    float gain_total = 0;
    requete *actuelle = LR->prem;
    while (actuelle && nb_requete)
    {
        int camion = -1;
        float cout = cout_requete_fin_trajet(*actuelle, a, &camion, graphe);
        if(camion == -1 && cout)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if(cout)
        {
            int taille_trajet = a.liste_camion[camion]->taille_trajet;
            int pos_camion = a.liste_camion[camion]->trajet[taille_trajet - 1];
            gain_total -= faire_course(a.liste_camion[camion], pos_camion, actuelle->origine, graphe, 0);
            gain_total -= faire_course(a.liste_camion[camion], actuelle->origine, actuelle->destination, graphe, 1);
            gain_total += actuelle->gain;
        }
        else
            gain_total -= actuelle->perte;

        actuelle = actuelle->suiv;
        nb_requete--;
    }

    if (nb_requete != 0 && actuelle == NULL)
    {
        printf("Attention la liste de requete contient moins de requete que le nombre indiqué en argument\n");
        return err;
    }
    a.gain_total = gain_total;
    return a;
}


// Algo amélioré
float insertion(requete r, entrepot a, int *id_camion, int *new_trajet, int *taille_new_trajet, float **graphe)
{
    int taille_ajout = 0;
    float meilleur_cout = MAX;
    float actuel_cout = 0;
    int position_insertion = 0;
    int indice_camion = 0;
    int *trajet_a_inserer = calloc(TAILLE_MAX_TRAJET, sizeof(int)); // la deviation du camion
    trajet_a_inserer[0] = r.origine;
    trajet_a_inserer[1] = r.destination;
    int bool = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        //PROBLEME SI ON EST A j = taille_trajet - 1 pour destination
        for (int j = 0; j < a.liste_camion[i]->taille_trajet - 1; j++)
        {
            taille_ajout = 0;
            actuel_cout = 0;
            bool = 0;
            int position_initiale = a.liste_camion[i]->trajet[j];
            int position_suivante = a.liste_camion[i]->trajet[j + 1];
            // On part de notre sommet initial (j), et on va effectuer la requête que l'on souhaite intégrer
            actuel_cout += cout_distance(graphe[position_initiale][r.origine]);
            actuel_cout += cout_distance(graphe[r.origine][r.destination]);
            /* Maintenant on souhaite faire revenir notre camion dans le trajet normal
             Soit on le fait revenir sur le sommet initial d'où il est parti
             Soit on le fait revenir sur le sommet suivant lequel il est parti,
             dans ce cas la le trajet entre j et le suivant doit être fait à vide
             Pour ne pas rater de requête.
            */
            if (!a.liste_camion[i]->charge[j]) // Sommet initial -> Sommet suivant fait à vide
            {

                actuel_cout += cout_distance(graphe[r.destination][position_suivante]);
                // On soustrait le trajet fait à vide puisqu'on ne le fait plus
                actuel_cout -= cout_distance(graphe[position_initiale][position_suivante]);

                bool = 1;
            }
            else if (a.liste_camion[i]->charge[j] == 1) // Sommet initial -> Sommet suivant fait à plein
            {
                // On revient simplement sur le sommet initial
                actuel_cout += cout_distance(graphe[r.destination][position_initiale]);
            }
            // Actuel cout = cout de la deviation, c'est la valeur qu'on cherche à minimiser
            if (actuel_cout < meilleur_cout)
            {
                memset(new_trajet, 0, TAILLE_MAX_TRAJET);
                meilleur_cout = actuel_cout;
                position_insertion = j + 1;
                memcpy(new_trajet, a.liste_camion[i]->trajet, position_insertion * sizeof(int));

                // Les if permettent d'eviter les lettre en double dans le trajet
                if (new_trajet[position_insertion - 1] != trajet_a_inserer[0])
                {
                    new_trajet[position_insertion] = trajet_a_inserer[0];
                    new_trajet[position_insertion + 1] = trajet_a_inserer[1];
                    position_insertion += 2;
                    taille_ajout++;
                }

                if (new_trajet[position_insertion - 1] != a.liste_camion[i]->trajet[j + bool])
                {
                    memcpy(new_trajet + position_insertion, a.liste_camion[i]->trajet + j + bool, (a.liste_camion[i]->taille_trajet - j) * sizeof(int));
                    taille_ajout++;
                }

                indice_camion = i;
            }
        }
    }
    if (meilleur_cout == MAX)
    {
        free(trajet_a_inserer);
        printf("Erreur, aucun cout pertinent n'a été trouvé\n");
        return -1;
    }
    free(trajet_a_inserer);
    *id_camion = indice_camion;
    *taille_new_trajet = a.liste_camion[indice_camion]->taille_trajet + taille_ajout;
    //printf("J'ai incrusté sur le sommet %d\n", position_insertion);
    //printf("Cela m'a couté s: %.2f\n", meilleur_cout);

    return meilleur_cout;
}

entrepot init_insertion(liste_requete *LR, entrepot a, int nb_requete, float **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.gain_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête à evaluer, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton
    float gain_total = 0;
    requete *actuelle = LR->prem;
    int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));

    for(int i = 0; i < a.nb_camion; i++)
    {
        a.liste_camion[i]->trajet[1] = a.id_entrepot;
        a.liste_camion[i]->taille_trajet++;
    }
    while (actuelle && nb_requete)
    {
        int camion = -1;
        int taille_new_trajet = 0;
        memset(new_trajet, 0, TAILLE_MAX_TRAJET);
        float cout = insertion(*actuelle, a, &camion, new_trajet, &taille_new_trajet, graphe);
        if((camion == -1 || !taille_new_trajet) && cout)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if(cout)
        {
            a.liste_camion[camion]->trajet = new_trajet;
            a.liste_camion[camion]->taille_trajet = taille_new_trajet;

            gain_total -= cout; //pas sure de ca, ca renvoie juste le cout de l'ajout ?
            gain_total += actuelle->gain;
        }
        else
            gain_total -= actuelle->perte;

        actuelle = actuelle->suiv;
        nb_requete--;
    }

    if (nb_requete != 0 && actuelle == NULL)
    {
        printf("Attention la liste de requete contient moins de requete que le nombre indiqué en argument\n");
        return err;
    }
    free(new_trajet);
    a.gain_total = gain_total;

    return a;
}

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

// brut force
int assignation_requete(entrepot a)
{
    int tab_requete[a.nb_requete];
    for (int i = 0; i < a.nb_requete; i++)
        tab_requete[i] = 1;

    // chaque indice du tab_requete représente le numéro de requête, le nombre dans cette case représente le numéro de camion qui traite la demande

    requete r[a.nb_requete];
    requete * copy = a.LR->prem;
    for (int i = 0; i < a.nb_requete; i++)
    {
        r[i].origine = copy->origine;
        r[i].destination = copy->destination;
        r[i].gain = copy->gain;
        copy = copy->suiv;
    }
    // On a chargé notre tableau de requete pour éviter de parcourir la liste chaînée en boucle


    int temp;
    for (int j = 1; j <= a.nb_requete; j++)
    {
        for (int i = 0; i < a.nb_requete - 1; i++)
        {
            temp = tab_requete[i];
            tab_requete[i] = tab_requete[i + 1];
            tab_requete[i + 1] = temp;
            affiche_tableau(tab_requete, a.nb_requete);
        }
    }
    // On incrémente le tableau à chaque fois pour obtenir toutes les combinaisons possibles
    incremente_tableau(tab_requete, a.nb_requete, a.nb_camion);
    return 0;
}
