#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib.h"

void charge_requete(FILE *f, liste_requete *LR)
{
    char origine, destination;
    int gain, perte;
    fscanf(f, "\norigine : %c", &origine);
    fscanf(f, "\ndestination : %c", &destination);
    fscanf(f, "\ngains : %d", &gain);
    fscanf(f, "\nperte : %d", &perte);
    ajout_requete(LR, origine, destination, gain, perte);
}

entrepot *charge_entrepots(char *nomfic, int * nb_entrepot)
{
    int nb_entrepot_buff = 0;
    entrepot *a = NULL;
    FILE *f = fopen("gestionnaire", "r");
    fscanf(f, "\nnombre d'entrepot :%d", &nb_entrepot_buff);
    *nb_entrepot = nb_entrepot_buff;
    a = malloc(sizeof(struct entrepot) * nb_entrepot_buff);

    for (int i = 0; i < nb_entrepot_buff; i++)
    {
        int nb_camion = 0;
        fscanf(f, "\nentrepot : %d", &a[i].id_entrepot);
        fscanf(f, "\nnombre de camion :%d\n", &nb_camion);
        fscanf(f, "\nnombre de requete :%d\n", &a[i].nb_requete);

        a[i].LR = malloc(sizeof(liste_requete));
        init_liste_requete(a[i].LR);
        for (int j = 0; j < a[i].nb_requete; j++)
        {
            charge_requete(f, a[i].LR);
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

void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte)
{
    requete *nouv = malloc(sizeof(requete));
    if (!nouv)
        exit(EXIT_FAILURE);

    nouv->destination = destination;
    nouv->gain = gain;
    nouv->perte = perte;
    nouv->origine = origine;

    nouv->prec = LR->dern;
    nouv->suiv = NULL;

    if (LR->dern)
        LR->dern->suiv = nouv;
    else
        LR->prem = nouv;

    LR->dern = nouv;
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

void affichage(liste_requete *LR)
{
    requete *actuelle = LR->prem;
    while (actuelle)
    {
        printf("origine : %c\ndestination : %c\ngains : %d\nperte : %d\n\n",
               actuelle->origine, actuelle->destination, actuelle->gain, actuelle->perte);
        actuelle = actuelle->suiv;
    }
}