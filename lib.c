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

entrepot *charge_entrepots(char *nomfic, int *nb_entrepot)
{
    int nb_entrepot_buff = 0;
    entrepot *a = NULL;
    FILE *f = fopen("gestionnaire", "r");
    fscanf(f, "\nnombre d'entrepot :%d", &nb_entrepot_buff);
    *nb_entrepot = nb_entrepot_buff;
    a = malloc(sizeof(struct entrepot) * nb_entrepot_buff);

    for (int i = 0; i < nb_entrepot_buff; i++)
    {
        char c = 'a';
        fscanf(f, "\nentrepot : %c", &c);
        fscanf(f, "\nnombre de camion :%d\n", &a[i].nb_camion);
        a[i].id_entrepot = c;

        a[i].liste_camion = malloc(a[i].nb_camion * sizeof(camion *));
        for (int j = 0; j < a[i].nb_camion; j++)
        {
            a[i].liste_camion[j] = malloc(sizeof(camion) * 64);
        }

        for (int j = 0; j < a[i].nb_camion; j++)
        {
            a[i].liste_camion[j]->id_entrepot = a[i].id_entrepot;
            a[i].liste_camion[j]->distance_parcouru = 0;
            a[i].liste_camion[j]->trajet = malloc(sizeof(char) * 64);
            a[i].liste_camion[j]->trajet[0] = '\0';
        }
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

void libere_acteur(entrepot a)
{
    liberation(a.LR);
    for (int i = 0; i < a.nb_camion; i++)
    {
        free(a.liste_camion[i]->trajet);
    }
    for (int i = 0; i < a.nb_camion; i++)
    {
        free(a.liste_camion[i]);
    }
    free(a.liste_camion);
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

void affichage_entrepot(entrepot a)
{
    printf("id_entrepot : %d\nnb_requete : %d\n", a.id_entrepot, a.nb_requete);
    for (int i = 0; i < a.nb_camion; i++)
    {
        printf("\nid_camion : %d\ndistance_parcouru : %d\nTrajet effectué :%s\n", i, a.liste_camion[i]->distance_parcouru, a.liste_camion[i]->trajet);
    }
    affichage(a.LR);
}

int gain_theorique(entrepot a)
{
    int gain = 0;
    requete *actuelle = a.LR->prem;
    while (actuelle)
    {
        gain += actuelle->gain;
        actuelle = actuelle->suiv;
    }
    return gain;
}

int course_basique(int **graphe, entrepot a)
{
    int gain = 0;
    requete *actuelle = a.LR->prem;
    int cpt_camion = 0;
    while (actuelle)
    {
        int cout_course = faire_course(a.liste_camion[cpt_camion], actuelle->origine, actuelle->destination, graphe);
        int cout_retour = faire_course(a.liste_camion[cpt_camion], actuelle->destination, a.id_entrepot, graphe);

        printf("%s\n", a.liste_camion[cpt_camion]->trajet);

        while (cout_course == 0 || cout_retour == 0) // Impossible de faire l'aller ou le retour, on passe au camion suivant
        {
            cpt_camion++;
            cpt_camion %= a.nb_camion;
            if (cpt_camion == a.nb_camion)
                return 0;
            cout_course = faire_course(a.liste_camion[cpt_camion], actuelle->origine, actuelle->destination, graphe);
            cout_retour = faire_course(a.liste_camion[cpt_camion], actuelle->destination, a.id_entrepot, graphe);
        }
        gain += actuelle->gain;
        gain -= cout_course + cout_retour;
        cpt_camion++;
        cpt_camion %= a.nb_camion;
        actuelle = actuelle->suiv;
    }
    return gain;
}

int calcul_cout_trajet(int d)
{
    return 0.8 * d + 20;
}

// Renvoi le coût de la course effectuée par le camion
int faire_course(camion *c, char origine, char destination, int **graphe)
{
    if (destination == c->id_entrepot && c->distance_parcouru + graphe[origine - 'A'][destination - 'A'] > DISTANCE_MAX)
    {
        return 0;
    }
    if ((c->distance_parcouru + graphe[origine - 'A'][destination - 'A'] + graphe[destination - 'A'][c->id_entrepot - 'A']) > DISTANCE_MAX)
    {
        return 0;
    }

    int t = strlen(c->trajet);

    if (t == 0)
    {
        c->trajet[t] = origine;
        c->trajet[t + 1] = destination;
        c->trajet[t + 2] = '\0';
    }
    else
    {
        c->trajet[t] = destination;
        c->trajet[t + 1] = '\0';
    }
    c->distance_parcouru += graphe[origine - 'A'][destination - 'A'];
    printf("%d\n", c->distance_parcouru);
    return calcul_cout_trajet(graphe[origine - 'A'][destination - 'A']);
}