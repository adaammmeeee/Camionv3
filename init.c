#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "init.h"

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