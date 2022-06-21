#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "chemins_gloutons.h"
#include "affichage.h"

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

        if (distance_parcouru + graphe[pos_camion][nouv.origine] + graphe[nouv.origine][nouv.destination] + graphe[nouv.destination][a.id_entrepot] <= DISTANCE_MAX)
        {
            *indice_camion = i;
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
        if (camion == -1 && cout)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if (cout)
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

float insertion(requete r, entrepot a, int *id_camion, int *new_trajet, int *new_charge, int *taille_new_trajet, float **graphe)
{
    float meilleur_cout = MAX;
    int insertion = -1;

    for(int cpt_camion = 0; cpt_camion < a.nb_camion; cpt_camion++)
    {
        for(int position_insertion = 0; position_insertion < a.liste_camion[cpt_camion]->taille_trajet; position_insertion++)
        {
            float cout_requete = 0;
            int taille_ajout = 0;
            int suite_trajet = position_insertion + 1;
            int trajet_vide = 1;
            
            int depart = a.liste_camion[cpt_camion]->trajet[position_insertion];
            int arrivee;
            //Si la requete n'est pas ajoutée à la fin du trajet et que le sommet position_insertion est l'origine d'un trajet vide
            //l'arrivée est le prochain sommet de la liste de trajet
            //sinon l'arrivee correspond au départ, un retour au point d'origine de la prochaine requête est nécessaire pour chercher la marchandise
            if ((position_insertion < a.liste_camion[cpt_camion]->taille_trajet - 1) && !a.liste_camion[cpt_camion]->charge[position_insertion])
                arrivee = a.liste_camion[cpt_camion]->trajet[position_insertion + 1];
            else
                arrivee = a.liste_camion[cpt_camion]->trajet[position_insertion];

            if(a.liste_camion[cpt_camion]->distance_parcouru + graphe[depart][r.origine] + graphe[r.origine][r.destination] + graphe[r.destination][arrivee] <= DISTANCE_MAX)
            {
                cout_requete += cout_distance(graphe[depart][r.origine]);
                cout_requete += cout_distance(graphe[r.origine][r.destination]);
                cout_requete += cout_distance(graphe[r.destination][arrivee]);
                cout_requete -= cout_distance(graphe[depart][arrivee]);

                if(cout_requete < meilleur_cout)
                {
                    for(int i = 0; i <= position_insertion; i++)
                        new_trajet[i] = a.liste_camion[cpt_camion]->trajet[i];
                    
                    for(int i = 0; i < position_insertion; i++)
                        new_charge[i] = a.liste_camion[cpt_camion]->charge[i];

                    if(r.origine != depart)
                    {
                        new_trajet[suite_trajet] = r.origine;
                        new_charge[suite_trajet - 1] = 0;
                        new_charge[suite_trajet] = 1;
                        suite_trajet++;
                        taille_ajout++;
                    }
                    else
                        new_charge[suite_trajet - 1] = 1;
                        
                    if(r.destination != arrivee)
                    {
                        new_trajet[suite_trajet] = r.destination;
                        new_charge[suite_trajet] = 0;
                        suite_trajet++;
                        taille_ajout++;
                    }
                    if(arrivee == depart)
                    {
                        trajet_vide = 0;
                        taille_ajout++;
                    }

                    for(int i = 0; i < (a.liste_camion[cpt_camion]->taille_trajet - (position_insertion + trajet_vide)); i++)
                        new_trajet[i + suite_trajet] = a.liste_camion[cpt_camion]->trajet[i + position_insertion + trajet_vide];

                    for(int i = 0; i < ((a.liste_camion[cpt_camion]->taille_trajet - 1) - (position_insertion + trajet_vide)); i++)
                        new_charge[i + suite_trajet] = a.liste_camion[cpt_camion]->charge[i + position_insertion + trajet_vide];
                    
                    *taille_new_trajet = a.liste_camion[cpt_camion]->taille_trajet + taille_ajout;
                    *id_camion = cpt_camion;
                    meilleur_cout = cout_requete;
                    insertion = position_insertion;
                }
            }
        }
    }
    if (meilleur_cout == MAX)
        return 0;

    //printf("J'ai incrusté sur le sommet %d\n", insertion);
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
    int *new_charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));

    while (actuelle && nb_requete)
    {
        int camion = -1;
        int taille_new_trajet = 0;
        memset(new_trajet, 0, TAILLE_MAX_TRAJET);
        memset(new_charge, 0, TAILLE_MAX_TRAJET - 1);
        float cout = insertion(*actuelle, a, &camion, new_trajet, new_charge, &taille_new_trajet, graphe);
        if ((camion == -1 || !taille_new_trajet) && cout)
        {
            printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
            return err;
        }
        else if (cout)
        {
            for(int i = 0; i < taille_new_trajet; i++)
                a.liste_camion[camion]->trajet[i] = new_trajet[i];

            for(int i = 0; i < taille_new_trajet - 1; i++)
                a.liste_camion[camion]->charge[i] = new_charge[i];
                
            a.liste_camion[camion]->taille_trajet = taille_new_trajet;

            gain_total -= cout;
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
    free(new_charge);
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

    for (int i = 0; i < taille_tab; i++)
    {
        int indice_requete = tab[i];
        gain_total += tab_requete[indice_requete].gain;
        gain_total -= cout_distance(graphe[pos_camion][tab_requete[indice_requete].origine]);
        gain_total -= cout_distance(graphe[tab_requete[indice_requete].origine][tab_requete[indice_requete].destination]);
        pos_camion = tab_requete[indice_requete].destination;
    }
    gain_total -= cout_distance(graphe[pos_camion][a.id_entrepot]);
    return gain_total;
}

int different_ordre(int *tab, int taille_tab, int *new_tab, int *case_noir, int cpt, requete *r, entrepot a, float **graphe)
{
    if (cpt == taille_tab)
    {
        affiche_tableau(new_tab, taille_tab);
        // printf("cout : %.2f\n",calcul_cout_tab_requete(new_tab, taille_tab, r, a, graphe));
        return 0;
    }

    for (int i = 0; i < taille_tab; i++)
    {
        if (case_noir[i] == 0)
        {
            new_tab[cpt] = tab[i];
            case_noir[i] = 1;
            different_ordre(tab, taille_tab, new_tab, case_noir, cpt + 1, r, a, graphe);
            case_noir[i] = 0;
        }
    }
    return -1;
}

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

int combinaison(int *tab, int *tab_ref, int n, int k, int index, int cpt, int **camion_requete, requete *r, entrepot a, float **graphe)
{
    // On a trouvé une combinaison !
    
    if (index == k)
    {
        printf("Nouvelle combinaison : \n");
        // Pour cette combinaison on test tous les ordres
        for (int i = 0; i < k; i++)
            camion_requete[i][0] = tab[i];

        // Maintenant que la première requete de chaque camion à été initialisé on rempli le reste du tableau avec les requete restante
        int requete_restante[n];
        int nb_requete_restante = 0;
        nb_requete_restante = rempli_difference(requete_restante, tab_ref, n, tab, k);
        // les indices d'insignation représente le numéro de la requete
        // La valeur des cases représente le camion qui prend la requête
        int *assignation = calloc(sizeof(int), nb_requete_restante);
        int nb_incrementation = k;
        for (int i = 0; i < nb_requete_restante-1; i++)
            nb_incrementation *= nb_incrementation;

        for (int z = 0; z < nb_incrementation; z++) 
        {
            // On passe par un tableau de position de curseur, pour pouvoir concaténer à chaque fois la requête à assigner bien à la fin sans parcourir en boucle le tableau
            int pos_curseur[k];
            for (int i = 0; i < k; i++)
                pos_curseur[i] = 1;

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
                printf("camion %d : ",i);
                affiche_tableau(camion_requete[i], pos_curseur[i]);
                different_ordre(camion_requete[i]+1, pos_curseur[i]-1, new_tab, case_noir, 0, r, a, graphe);
                printf("\n");
                memset(camion_requete[i] + 1, 0, sizeof(int) * (n - 1));
            }
            printf("\n\n");
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

    int n = 5;
    int k = 3;

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
    // affiche_tableau(new_tab, k);
    //  calcul_meilleur_combinaison(camion_requete[i], pos_curseur[i]);

    // On incrémente le tableau à chaque fois pour obtenir toutes les combinaisons possibles
    for (int i = 0; i < a.nb_camion; i++)
    {
        free(camion_requete[i]);
    }
    free(camion_requete);
    return 0;
}
