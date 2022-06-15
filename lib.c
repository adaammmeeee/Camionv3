#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib.h"

int charge_requete(FILE *f, liste_requete *LR, int **graphe, char id_entrepot)
{
    char origine, destination;
    int gain, perte;
    fscanf(f, "\norigine : %c", &origine);
    fscanf(f, "\ndestination : %c", &destination);
    fscanf(f, "\ngains : %d", &gain);
    fscanf(f, "\nperte : %d", &perte);
    return ajout_requete(LR, origine, destination, gain, perte, graphe, id_entrepot);
}

int **charge_graphe(char *nomfic, int nb_entrepots)
{
    int **graphe = calloc(nb_entrepots, sizeof(int *));

    for (int i = 0; i < nb_entrepots; i++)
    {
        graphe[i] = calloc(nb_entrepots, sizeof(int));
    }

    FILE *f = fopen(nomfic, "r");
    char nombre[10];

    for (int i = 0; i < nb_entrepots; i++)
    {
        for (int j = 0; j < nb_entrepots; j++)
        {
            int cpt = 0;
            char car = fgetc(f);
            while (!(car == '\n' || car == ','))
            {
                nombre[cpt] = car;
                car = fgetc(f);
                cpt++;
            }
            nombre[cpt] = '\0';

            graphe[i][j] = atoi(nombre);
        }
    }

    fclose(f);
    return graphe;
}

int charge_nombre_entrepots(char *nomfic)
{
    int nb_entrepot_buff = 0;
    FILE *f = fopen("gestionnaire", "r");
    fscanf(f, "\nnombre d'entrepot :%d", &nb_entrepot_buff);
    fclose(f);
    return nb_entrepot_buff;
}

entrepot *charge_entrepots(char *nomfic, int **graphe)
{
    int nb_entrepot_buff = 0;
    entrepot *a = NULL;
    FILE *f = fopen("gestionnaire", "r");
    fscanf(f, "\nnombre d'entrepot :%d", &nb_entrepot_buff);
    a = calloc(nb_entrepot_buff, sizeof(struct entrepot));
    if (!a)
    {
        printf("Problème d'allocation : fonction %s", __FUNCTION__);
        return NULL;
    }

    for (int i = 0; i < nb_entrepot_buff; i++)
    {
        char buf = 'a';
        fscanf(f, "\nentrepot : %c", &buf);
        fscanf(f, "\nnombre de camion :%d\n", &a[i].nb_camion);
        a[i].id_entrepot = buf;
        a[i].gain_total = 0;

        a[i].liste_camion = calloc(a[i].nb_camion, sizeof(camion *));
        for (int j = 0; j < a[i].nb_camion; j++)
        {
            a[i].liste_camion[j] = calloc(NB_MAX_CAMION, sizeof(camion));
        }

        for (int j = 0; j < a[i].nb_camion; j++)
        {
            // a[i].liste_camion[j]->id_entrepot = a[i].id_entrepot; c'était pour debug
            a[i].liste_camion[j]->distance_parcouru = 0;
            a[i].liste_camion[j]->trajet = calloc(TAILLE_MAX_TRAJET, sizeof(char));
            a[i].liste_camion[j]->trajet[0] = a[i].id_entrepot;
            a[i].liste_camion[j]->trajet[1] = '\0';
            a[i].liste_camion[j]->charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(char));
            // a[i].liste_camion[j]->id_camion = j + '1'; // pareil c'est pour debug
        }
        fscanf(f, "\nnombre de requete :%d\n", &a[i].nb_requete);

        a[i].LR = calloc(1, sizeof(liste_requete));
        init_liste_requete(a[i].LR);
        for (int j = 0; j < a[i].nb_requete; j++)
        {
            charge_requete(f, a[i].LR, graphe, a[i].id_entrepot);
        }
    }

    fclose(f);
    return a;
}

void init_liste_requete(liste_requete *LR)
{
    LR->dern = NULL;
    LR->prem = NULL;
}

int ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int **graphe, char id_entrepot)
{
    requete *nouv = calloc(1, sizeof(requete));
    if (!nouv)
    {
        printf("Erreur dans l'allocation : fonction %s\n", __FUNCTION__);
        return 1;
    }

    nouv->destination = destination;
    nouv->gain = gain;
    nouv->perte = perte;
    nouv->prix_propose_vente = 0;
    nouv->origine = origine;
    nouv->id_entrepot = id_entrepot;

    nouv->prec = NULL;
    nouv->suiv = NULL;

    if (!LR->dern)
    {
        LR->prem = nouv;
        LR->dern = nouv;
        return 0;
    }

    requete *comparateur = LR->dern;
    int test = 0;
    // C'est ici qu'on parcourt notre liste chainée pour insérer notre nouvelle requete conformément au tri
    while (graphe[nouv->origine - 'A'][nouv->destination - 'A'] > graphe[comparateur->origine - 'A'][comparateur->destination - 'A'])
    {
        if (comparateur->prec)
        {
            comparateur = comparateur->prec;
        }
        else
        {
            test = 1;
            break;
        }
    }
    // à ce niveau là le gain de comparateur est inferieur à celui de nouv
    if (!comparateur->suiv && test == 0) // Dans le cas où on insère en bout de liste
    {
        comparateur->suiv = nouv;
        nouv->prec = comparateur;
        LR->dern = nouv;
    }

    else if (!comparateur->prec && test == 1) // Dans le cas où on insère en début de liste
    {
        comparateur->prec = nouv;
        nouv->suiv = comparateur;
        LR->prem = nouv;
    }

    else
    {
        nouv->suiv = comparateur->suiv;
        nouv->prec = comparateur;
        comparateur->suiv->prec = nouv;
        comparateur->suiv = nouv;
    }

    return 0;
}

void liberation(liste_requete *LR)
{
    requete *temporaire;
    requete *actuelle = LR->prem;
    while (actuelle)
    {
        temporaire = actuelle;
        actuelle = actuelle->suiv;
        free(temporaire);
    }
    free(LR);
}

void libere_acteur(entrepot a)
{
    liberation(a.LR);
    for (int i = 0; i < a.nb_camion; i++)
    {
        free(a.liste_camion[i]->trajet);
        free(a.liste_camion[i]->charge);
    }
    for (int i = 0; i < a.nb_camion; i++)
    {
        free(a.liste_camion[i]);
    }
    free(a.liste_camion);
}

void affichage_requete(liste_requete *LR)
{
    requete *actuelle = LR->prem;
    while (actuelle)
    {
        printf("origine : %c\ndestination : %c\ngains : %d\nperte : %d\n\n",
               actuelle->origine, actuelle->destination, actuelle->gain, actuelle->perte);
        actuelle = actuelle->suiv;
    }
}

void affichage_entrepot(entrepot a)
{
    printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
    for (int i = 0; i < a.nb_camion; i++)
    {
        printf("\nid_camion : %d\ndistance_parcouru : %d\nTrajet effectué :%s\n", i, a.liste_camion[i]->distance_parcouru, a.liste_camion[i]->trajet);
    }
    affichage_requete(a.LR);
}

int calcul_cout_trajet(int d)
{
    return d ? (0.8 * d + 20) : (0);
}

// Renvoi la distance entre un camion cam et un sommet du graphe (originie_requete)
int proximite(int **graphe, camion cam, char origine_requete)
{
    int taille_trajet = strlen(cam.trajet);
    char pos_camion = cam.trajet[taille_trajet - 1];

    return graphe[pos_camion - 'A'][origine_requete - 'A'];
}

void fusion(int **graphe, char origine_requete, camion **liste_camion, int deb1, int fin1, int fin2)
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

void tri_fusion_camion_proximite(int **graphe, char origine_requete, camion **liste_camion, int deb, int fin)
{
    if (deb < fin)
    {
        int milieu = (fin + deb) / 2;
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, deb, milieu);
        tri_fusion_camion_proximite(graphe, origine_requete, liste_camion, milieu + 1, fin);
        fusion(graphe, origine_requete, liste_camion, deb, milieu, fin);
    }
}

int faire_course(camion *c, char origine, char destination, int **graphe, int plein)
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
    return calcul_cout_trajet(graphe[origine - 'A'][destination - 'A']);
}

entrepot evaluation_meilleure_solution(liste_requete *LR, entrepot a, int nb_requete, int **graphe)
{
    // Gestion d'erreur
    entrepot err;
    err.gain_total = -1;

    if (!LR || nb_requete == 0)
    {
        printf("Aucune requête a evalué, error in %s\n", __FUNCTION__);
        return err;
    }

    // Debut du glouton miam
    int gain_total = 0;
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
            int distance_parcouru = a.liste_camion[i]->distance_parcouru;

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

int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, int **graphe)
{
    int cout = 0;
    tri_fusion_camion_proximite(graphe, nouv.origine, a.liste_camion, 0, a.nb_camion - 1);
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = strlen(a.liste_camion[i]->trajet);
        int pos_camion = a.liste_camion[i]->trajet[taille_trajet - 1] - 'A';
        int origine = nouv.origine - 'A';
        int destination = nouv.destination - 'A';
        int distance_parcouru = a.liste_camion[i]->distance_parcouru;
        *indice_camion = i;

        if (distance_parcouru + graphe[pos_camion][origine] + graphe[origine][destination] + graphe[destination][a.id_entrepot - 'A'] <= DISTANCE_MAX)
        {
            cout += calcul_cout_trajet(graphe[pos_camion][origine]);
            cout += calcul_cout_trajet(graphe[origine][destination]);
            cout += calcul_cout_trajet(graphe[destination][a.id_entrepot - 'A']);
            break;
        }
    }
    return cout;
}

entrepot retour_a_la_casa(entrepot a, int **graphe)
{
    int taille = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        taille = strlen(a.liste_camion[i]->trajet);
        char origine = a.liste_camion[i]->trajet[taille - 1];
        a.gain_total -= faire_course(a.liste_camion[i], origine, a.id_entrepot, graphe, 0);
    }
    return a;
}

requete copie_requete(requete r, int prix_propose)
{
    requete nouv;
    nouv.id_entrepot = r.id_entrepot;
    nouv.origine = r.origine;
    nouv.destination = r.destination;
    nouv.gain = r.gain;
    nouv.prix_propose_vente = prix_propose;
    nouv.perte = r.perte;
    nouv.id_entrepot = r.id_entrepot;

    return nouv;
}

int insertion(int *id_camion, char *new_trajet, entrepot a, requete r, int **graphe)
{
    int meilleur_cout = MAX;
    int actuel_cout = 0;
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
            actuel_cout += calcul_cout_trajet(graphe[position_initiale - 'A'][r.origine - 'A']);
            actuel_cout += calcul_cout_trajet(graphe[r.origine - 'A'][r.destination - 'A']);
            /* Maintenant on souhaite faire revenir notre camion dans le trajet normal
             Soit on le fait revenir sur le sommet initial d'où il est parti
             Soit on le fait revenir sur le sommet suivant lequel il est parti,
             dans ce cas la le trajet entre j et le suivant doit être fait à vide
             Pour ne pas rater de requête.
            */
            if (a.liste_camion[i]->charge[j] == '0') // Sommet initial -> Sommet suivant fait à vide
            {
                actuel_cout += calcul_cout_trajet(graphe[r.destination - 'A'][position_suivante - 'A']);
                
                // On soustrait le trajet fait à vide puisqu'on ne le fait plus
                actuel_cout -= calcul_cout_trajet(graphe[position_initiale - 'A'][position_suivante - 'A']);
                bool = 1;
            }
            else // Sommet initial -> Sommet suivant fait à plein
            {
                // On revient simplement sur le sommet initial
                actuel_cout += calcul_cout_trajet(graphe[r.destination - 'A'][position_initiale - 'A']);
            }
            // Actuel cout = cout de la deviation, c'est la valeur qu'on cherche à minimiser
            if (actuel_cout < meilleur_cout)
            {
                memset(new_trajet, 0, TAILLE_MAX_TRAJET);
                meilleur_cout = actuel_cout;
                position_insertion = j;
                strncpy(new_trajet, a.liste_camion[i]->trajet, position_insertion);
                strcat(new_trajet, trajet_a_inserer);
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
    return actuel_cout;
}

entrepot *enchere_echange(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{

    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        char entrepot_demande = rv[cpt_requete].id_entrepot;
        int indice_e_demande = entrepot_demande - 'A';
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        int cout_requete_min = rv[cpt_requete].prix_propose_vente;

        for (int cpt_entrepot = 0; cpt_entrepot < nb_entrepot; cpt_entrepot++)
        {
            char entrepot_offre = a[cpt_entrepot].id_entrepot;
            if (entrepot_demande != entrepot_offre)
            {
                int camion_offre = -1;
                int indice_e_offre = entrepot_offre - 'A';
                int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_offre], &camion_offre, graphe);
                if (cout_requete && cout_requete < cout_requete_min)
                {
                    cout_requete_min = cout_requete;
                    indice_e_offre_min = indice_e_offre;
                    indice_c_offre_min = camion_offre;
                    cpt_offre++;
                }
            }
        }

        if (cpt_offre)
        {
            int taille_trajet = strlen(a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet);
            char pos_camion = a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet[taille_trajet - 1];

            a[indice_e_offre_min].gain_total -= faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], pos_camion, rv[cpt_requete].origine, graphe, 0);
            a[indice_e_offre_min].gain_total -= faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], rv[cpt_requete].origine, rv[cpt_requete].destination, graphe, 1);
            a[indice_e_offre_min].gain_total += cout_requete_min + 1;
            a[indice_e_demande].gain_total -= cout_requete_min + 1;
        }
        else
        {
            int camion_demande = -1;
            cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_demande], &camion_demande, graphe);
            int indice_c_demande = camion_demande;
            int taille_trajet = strlen(a[indice_e_demande].liste_camion[indice_c_demande]->trajet);
            char pos_camion = a[indice_e_demande].liste_camion[indice_c_demande]->trajet[taille_trajet - 1];

            a[indice_e_demande].gain_total -= faire_course(a[indice_e_demande].liste_camion[indice_c_demande], pos_camion, rv[cpt_requete].origine, graphe, 0);
            a[indice_e_demande].gain_total -= faire_course(a[indice_e_demande].liste_camion[indice_c_demande], rv[cpt_requete].origine, rv[cpt_requete].destination, graphe, 1);
        }
        a[indice_e_demande].gain_total += rv[cpt_requete].gain;
    }

    return a;
}