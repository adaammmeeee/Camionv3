#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "chemins_gloutons.h"

float faire_course(camion *c, char origine, char destination, float **graphe, int plein)
{
    if (origine == destination)
        return 0;
    int taille = strlen(c->trajet);

    if (taille == 0)
    {
        printf("Erreur probable dans l'initialisation des trajet, le trajet doit au moins contenir l'entrepot de départ\n");
        return -1;
    }
    else
    {
        c->trajet[taille] = destination;
        c->trajet[taille + 1] = '\0';
        c->charge[taille - 1] = plein + '0';
    }

    c->distance_parcouru += graphe[origine - 'A'][destination - 'A'];
    return cout_distance(graphe[origine - 'A'][destination - 'A']);
}

// Renvoi la distance entre un camion cam et un sommet du graphe (originie_requete)
float proximite(float **graphe, camion cam, char origine_requete)
{
    int taille_trajet = strlen(cam.trajet);
    char pos_camion = cam.trajet[taille_trajet - 1];

    return graphe[pos_camion - 'A'][origine_requete - 'A'];
}

void fusion(float **graphe, char origine_requete, camion **liste_camion, int deb1, int fin1, int fin2)
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

void tri_fusion_camion_proximite(float **graphe, char origine_requete, camion **liste_camion, int deb, int fin)
{
    if (deb < fin)
    {
        int milieu = (fin + deb) / 2;
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, deb, milieu);
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, milieu + 1, fin);
        fusion(graphe, origine_requete, liste_camion, deb, milieu, fin);
    }
}

entrepot evaluation_meilleure_solution(liste_requete *LR, entrepot a, int nb_requete, float **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.gain_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête a evalué, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton 
    float gain_total = 0;
    requete *actuelle = LR->prem;
    while (actuelle && nb_requete)
    {
        tri_fusion_camion_proximite(graphe, a.id_entrepot, a.liste_camion, 0, a.nb_camion - 1);
        for (int i = 0; i < a.nb_camion; i++)
        {
            int taille_trajet = strlen(a.liste_camion[i]->trajet);
            int pos_camion = a.liste_camion[i]->trajet[taille_trajet - 1] - 'A';
            int origine = actuelle->origine - 'A';
            int destination = actuelle->destination - 'A';
            float distance_parcouru = a.liste_camion[i]->distance_parcouru;

            if (distance_parcouru + graphe[pos_camion][origine] + graphe[origine][destination] + graphe[destination][a.id_entrepot - 'A'] <= DISTANCE_MAX)
            {
                // Trajet à vide, le camion quitte sa position pour aller vers l'origine de la requete
                gain_total -= faire_course(a.liste_camion[i], a.liste_camion[i]->trajet[taille_trajet - 1], actuelle->origine, graphe, 0);
                // Trajet plein le camion part de l'origine vers la destination de la requête
                gain_total -= faire_course(a.liste_camion[i], actuelle->origine, actuelle->destination, graphe, 1);
                gain_total += actuelle->gain;
                break;
            }
        }
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

int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, float **graphe)
{
    float cout = 0;
    tri_fusion_camion_proximite(graphe, nouv.origine, a.liste_camion, 0, a.nb_camion - 1);
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = strlen(a.liste_camion[i]->trajet);
        int pos_camion = a.liste_camion[i]->trajet[taille_trajet - 1] - 'A';
        int origine = nouv.origine - 'A';
        int destination = nouv.destination - 'A';
        float distance_parcouru = a.liste_camion[i]->distance_parcouru;
        *indice_camion = i;

        if (distance_parcouru + graphe[pos_camion][origine] + graphe[origine][destination] + graphe[destination][a.id_entrepot - 'A'] <= DISTANCE_MAX)
        {
            cout += cout_distance(graphe[pos_camion][origine]);
            cout += cout_distance(graphe[origine][destination]);
            cout += cout_distance(graphe[destination][a.id_entrepot - 'A']);
            break;
        }
    }
    return cout;
}

// Algo amélioré
float insertion(int *id_camion, char *new_trajet, entrepot a, requete r, float **graphe)
{
    float meilleur_cout = MAX;
    float actuel_cout = 0;
    int position_insertion = 0;
    int indice_camion = 0;
    char *trajet_a_inserer = calloc(8, sizeof(char)); // la deviation du camion
    trajet_a_inserer[0] = r.origine;
    trajet_a_inserer[1] = r.destination;
    int bool = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = strlen(a.liste_camion[i]->trajet);
        for (int j = 0; j < taille_trajet - 1; j++)
        {
            actuel_cout = 0;
            bool = 0;
            char position_initiale = a.liste_camion[i]->trajet[j];
            char position_suivante = a.liste_camion[i]->trajet[j + 1];
            // On part de notre sommet initial (j), et on va effectuer la requête que l'on souhaite intégrer
            actuel_cout += cout_distance(graphe[position_initiale - 'A'][r.origine - 'A']);
            actuel_cout += cout_distance(graphe[r.origine - 'A'][r.destination - 'A']);
            /* Maintenant on souhaite faire revenir notre camion dans le trajet normal
             Soit on le fait revenir sur le sommet initial d'où il est parti
             Soit on le fait revenir sur le sommet suivant lequel il est parti,
             dans ce cas la le trajet entre j et le suivant doit être fait à vide
             Pour ne pas rater de requête.
            */
            if (a.liste_camion[i]->charge[j] == '0') // Sommet initial -> Sommet suivant fait à vide
            {


                actuel_cout += cout_distance(graphe[r.destination - 'A'][position_suivante - 'A']);
                // On soustrait le trajet fait à vide puisqu'on ne le fait plus
                actuel_cout -= cout_distance(graphe[position_initiale - 'A'][position_suivante - 'A']);

                bool = 1;
            }
            else if (a.liste_camion[i]->charge[j] == '1') // Sommet initial -> Sommet suivant fait à plein
            {
                // On revient simplement sur le sommet initial
                actuel_cout += cout_distance(graphe[r.destination - 'A'][position_initiale - 'A']);
            }
            // Actuel cout = cout de la deviation, c'est la valeur qu'on cherche à minimiser
            if (actuel_cout < meilleur_cout)
            {
                memset(new_trajet, 0, TAILLE_MAX_TRAJET);
                meilleur_cout = actuel_cout;
                position_insertion = j;
                strncpy(new_trajet, a.liste_camion[i]->trajet, position_insertion + 1);

                // Les if permettent d'eviter les lettre en double dans le trajet
                if (new_trajet[strlen(new_trajet) - 1] != trajet_a_inserer[0])
                    strcat(new_trajet, trajet_a_inserer);

                if (new_trajet[strlen(new_trajet) - 1] != a.liste_camion[i]->trajet[position_insertion + bool])
                    strcat(new_trajet, a.liste_camion[i]->trajet + position_insertion + bool);
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
    printf("J'ai incrusté sur le sommet %d\n", position_insertion);
    printf("Cela m'a couté s: %.2f\n", meilleur_cout);
    return meilleur_cout;
}
