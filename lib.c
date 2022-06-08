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

int **charge_graphe(char *nomfic, int nb_entrepots)
{
    int **graphe = malloc(nb_entrepots * sizeof(int *));

    for (int i = 0; i < nb_entrepots; i++)
    {
        graphe[i] = malloc(nb_entrepots * sizeof(int));
    }

    FILE *f = fopen(nomfic, "r");
    int cpt = 0;
    char nombre[10];
    memset(nombre, 0, 10);

    for (int i = 0; i < nb_entrepots; i++)
    {
        for (int j = 0; j < nb_entrepots; j++)
        {
            nombre[0] = fgetc(f);
            while (1)
            {
                if (nombre[cpt] == '\n' || nombre[cpt] == ',')
                {
                    nombre[cpt] = '\0';
                    break;
                }
                cpt++;
                nombre[cpt] = fgetc(f);
            }

            graphe[i][j] =  atoi(nombre);
            memset(nombre, 0, 10);
            cpt = 0;
        }
    }

    fclose(f);
    return graphe;
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
            a[i].liste_camion[j]->trajet[0] = a[i].id_entrepot;
            a[i].liste_camion[j]->trajet[1] = '\0';
            a[i].liste_camion[j]->id_camion = j;
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

    nouv->prec = NULL;
    nouv->suiv = NULL;

    if (!LR->dern)
    {
        LR->prem = nouv;
        LR->dern = nouv;
        return;
    }

    requete *comparateur = LR->dern;
    int test = 0;
    while (nouv->gain < comparateur->gain)
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

        printf("Le camion %d de l'entrepot %c fait le trajet %s\n", cpt_camion, a.id_entrepot, a.liste_camion[cpt_camion]->trajet);
        int premier_test = cpt_camion;
        while (cout_course == 0 || cout_retour == 0) // Impossible de faire l'aller ou le retour, on passe au camion suivant
        {
            cpt_camion++;
            cpt_camion %= a.nb_camion;
            if (cpt_camion == premier_test)
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

    liste_tmp = malloc((fin1 - deb1 + 1) * sizeof(camion));

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

// Renvoi le coût de la course effectuée par le camion
int faire_course(camion *c, char origine, char destination, int **graphe)
{

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
    return calcul_cout_trajet(graphe[origine - 'A'][destination - 'A']);
}

int evaluation_meilleure_solution(entrepot a, int nb_camion, int **graphe)
{
    int gain_total = 0;
    int old_gain = 0;
    requete *actuelle = a.LR->prem;
    while (actuelle)
    {
        printf("ccccc\n");
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
                old_gain = gain_total;
                printf("le camion %d a fait l'itinéraire %c->%c->%c\n", a.liste_camion[i]->id_camion, pos_camion + 'A', origine + 'A', destination + 'A');
                gain_total -= faire_course(a.liste_camion[i], a.liste_camion[i]->trajet[taille_trajet - 1], actuelle->origine, graphe);
                gain_total -= faire_course(a.liste_camion[i], actuelle->origine, actuelle->destination, graphe);
                gain_total += actuelle->gain;
                printf("le cout apporté est le suivant : %d\n", gain_total - old_gain);
                break;
            }
        }
        actuelle = actuelle->suiv;
    }
    return gain_total;
}