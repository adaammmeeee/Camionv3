#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "structures.h"
#include "init.h"

int charge_requete(FILE *f, liste_requete *LR, float **graphe, int id_entrepot)
{
    int origine, destination;
    float gain, perte;
    char c = 0;
    while (!isdigit(c))
    {
        c = fgetc(f);
    }
    char buff[256];
    memset(buff, 0, sizeof(buff));
    int cpt = 0;
    while (isdigit(c))
    {
        buff[cpt] = c;
        cpt++;
        c = fgetc(f);
    }
    origine = atoi(buff);
    // On a recupéré l'origine de la requete
    memset(buff, 0, 256);

    while (!isdigit(c))
    {
        c = fgetc(f);
    }
    cpt = 0;
    while (isdigit(c))
    {
        buff[cpt] = c;
        cpt++;
        c = fgetc(f);
    }
    destination = atoi(buff);
    // On a recupéré la destination de la requete

    memset(buff, 0, 256);

    while (!isdigit(c))
    {
        c = fgetc(f);
    }
    cpt = 0;
    while (isdigit(c) || c == '.')
    {
        buff[cpt] = c;
        cpt++;
        c = fgetc(f);
    }
    gain = atof(buff);
    // On a recupéré le gain de la requete

    memset(buff, 0, 256);

    while (!isdigit(c))
    {
        c = fgetc(f);
    }
    cpt = 0;
    while (isdigit(c) || c == '.')
    {
        buff[cpt] = c;
        cpt++;
        c = fgetc(f);
    }
    perte = atof(buff);
    // On a recupéré la perte de la requete

    return ajout_requete(LR, origine, destination, gain, perte, graphe, id_entrepot);
}

float **charge_graphe(char *nomfic, int * nb_entrepot)
{
    FILE *f = fopen(nomfic, "r");
    char c = 0;
    int nb_entrepots = 1;
    while ((c = fgetc(f)) != '\n')
    {
        if (c == ',')
            nb_entrepots++;
    }
    *nb_entrepot = nb_entrepots;
    fseek(f, 0, SEEK_SET);

    float **graphe = calloc(nb_entrepots, sizeof(int *));

    for (int i = 0; i < nb_entrepots; i++)
    {
        graphe[i] = calloc(nb_entrepots, sizeof(int));
    }

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

            graphe[i][j] = atof(nombre);
        }
    }

    fclose(f);
    return graphe;
}

entrepot *charge_entrepots(char *nomfic, float **graphe)
{
    int nb_entrepot_buff = 0;
    entrepot *a = NULL;
    FILE *f = fopen(nomfic, "r");
    fscanf(f, "\nnombre d'entrepot :%d\n\n", &nb_entrepot_buff);
    a = calloc(nb_entrepot_buff, sizeof(struct entrepot));
    if (!a)
    {
        printf("Problème d'allocation : fonction %s", __FUNCTION__);
        return NULL;
    }

    for (int i = 0; i < nb_entrepot_buff; i++)
    {
        char c = 0;
        while (!isdigit(c))
        {
            c = fgetc(f);
        }
        char buff[256];
        memset(buff, 0, 256);
        int cpt = 0;
        while (isdigit(c))
        {
            buff[cpt] = c;
            cpt++;
            c = fgetc(f);
        }
        a[i].id_entrepot = atoi(buff);
        // Ici on a recupéré l'id de l'entrepot
        memset(buff, 0, 256);

        while (!isdigit(c))
        {
            c = fgetc(f);
        }
        cpt = 0;
        while (isdigit(c))
        {
            buff[cpt] = c;
            cpt++;
            c = fgetc(f);
        }
        a[i].nb_camion = atoi(buff);
        // Ici on a recupéré le nombre de camion
        memset(buff, 0, 256);

        while (!isdigit(c))
        {
            c = fgetc(f);
        }
        cpt = 0;
        while (isdigit(c))
        {
            buff[cpt] = c;
            cpt++;
            c = fgetc(f);
        }
        a[i].nb_requete = atoi(buff);
        // Ici on a recupéré le nombre de requetes

        // On passe à l'initialisation des autres champs de l'entrepots
        a[i].gain_total = 0;
        a[i].liste_camion = calloc(a[i].nb_camion, sizeof(camion *));
        for (int j = 0; j < a[i].nb_camion; j++)
        {
            a[i].liste_camion[j] = calloc(NB_MAX_CAMION, sizeof(camion));
        }

        for (int j = 0; j < a[i].nb_camion; j++)
        {
            a[i].liste_camion[j]->id_camion = j;
            a[i].liste_camion[j]->distance_parcouru = 0;
            a[i].liste_camion[j]->taille_trajet = 1;
            a[i].liste_camion[j]->trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
            a[i].liste_camion[j]->trajet[0] = a[i].id_entrepot;
            a[i].liste_camion[j]->charge = calloc(TAILLE_MAX_TRAJET - 1, sizeof(int));
        }

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

int ajout_requete(liste_requete *LR, int origine, int destination, float gain, float perte, float **graphe, int id_entrepot)
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
    while (graphe[nouv->origine][nouv->destination] > graphe[comparateur->origine][comparateur->destination])
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