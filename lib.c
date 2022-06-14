#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib.h"

void charge_requete(FILE *f, liste_requete *LR, int ** graphe, char id_entrepot)
{
    char origine, destination;
    int gain, perte;
    fscanf(f, "\norigine : %c", &origine);
    fscanf(f, "\ndestination : %c", &destination);
    fscanf(f, "\ngains : %d", &gain);
    fscanf(f, "\nperte : %d", &perte);
    ajout_requete(LR, origine, destination, gain, perte, graphe, id_entrepot);
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

            graphe[i][j] = atoi(nombre);
            memset(nombre, 0, 10);
            cpt = 0;
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
    a = malloc(sizeof(struct entrepot) * nb_entrepot_buff);

    for (int i = 0; i < nb_entrepot_buff; i++)
    {
        char c = 'a';
        fscanf(f, "\nentrepot : %c", &c);
        fscanf(f, "\nnombre de camion :%d\n", &a[i].nb_camion);
        a[i].id_entrepot = c;
        a[i].gain_total = 0;

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
            a[i].liste_camion[j]->charge = malloc(sizeof(char) * 64);
            memset(a[i].liste_camion[j]->charge, 0, 64);
            a[i].liste_camion[j]->id_camion = j + '1';
        }
        fscanf(f, "\nnombre de requete :%d\n", &a[i].nb_requete);

        a[i].LR = malloc(sizeof(liste_requete));
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

void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int ** graphe, char id_entrepot)
{
    requete *nouv = malloc(sizeof(requete));
    if (!nouv)
        exit(EXIT_FAILURE);

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
        return;
    }

    requete *comparateur = LR->dern;
    int test = 0;
    while (nouv->gain - graphe[nouv->origine - 'A'][nouv->destination - 'A'] > comparateur->gain - graphe[comparateur->origine - 'A'][comparateur->destination - 'A'])
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

int calcul_cout_trajet(int d)
{
    return d ? (0.8 * d + 20) : (0);
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

int faire_course(camion *c, char origine, char destination, int **graphe, int plein)
{
    if (origine == destination)
        return 0;
    int taille = strlen(c->trajet);

    if (taille == 0)
    {
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

entrepot evaluation_meilleure_solution(liste_requete * LR, entrepot a, int nb_requete, int **graphe)
{
    int gain_total = 0;
    // int old_gain = 0;
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
                // Trajet à vide
                gain_total -= faire_course(a.liste_camion[i], a.liste_camion[i]->trajet[taille_trajet - 1], actuelle->origine, graphe, 0);
                // Trajet plein
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
        exit(1);
    }
    a.gain_total = gain_total;
    return a;
}


int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, int **graphe)
{
    int cout = 0;
    tri_fusion_camion_proximite(graphe, nouv.origine, a.liste_camion, 0, a.nb_camion - 1);
    for(int i = 0; i < a.nb_camion; i++)
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


void retour_a_la_casa(entrepot a, int ** graphe)
{
    int taille = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        taille = strlen(a.liste_camion[i]->trajet);
        char origine = a.liste_camion[i]->trajet[taille-1];
        faire_course(a.liste_camion[i],origine,a.id_entrepot,graphe,0);
    }
}


requete copie_requete(requete * r, int prix_propose)
{
    requete nouv;
    nouv.id_entrepot = r->id_entrepot;
    nouv.origine = r->origine;
    nouv.destination = r->destination;
    nouv.gain = r->gain;
    nouv.prix_propose_vente = prix_propose;
    nouv.perte = r->perte;
    nouv.id_entrepot = r->id_entrepot;
    return nouv;
}

int insertion(int * indice_trajet, char * id_camion, entrepot a, requete r, int **graphe)
{
    int meilleur_cout = MAX;
    int actuel_cout = 0;
    int position_insertion = 0;
    int indice_camion = 0;
    for (int i = 0; i < a.nb_camion; i++)
    {
        int taille_trajet = strlen(a.liste_camion[i]->trajet);
        for (int j = 0; j < taille_trajet-1; j++)
        {
            char position = a.liste_camion[i]->trajet[j];
            actuel_cout += calcul_cout_trajet(graphe[position-'A'][r.origine-'A']);
            actuel_cout += calcul_cout_trajet(graphe[r.origine- 'A'][r.destination- 'A']);
            actuel_cout += calcul_cout_trajet(graphe[r.destination- 'A'][position-'A']);
            // Actuel cout = cout de la deviation, c'est la valeur qu'on cherche à minimiser
            if (actuel_cout < meilleur_cout)
            {
                meilleur_cout = actuel_cout;
                position_insertion = j;
                indice_camion = i;
            }
            else
            actuel_cout = 0;
        }
    }
    if (meilleur_cout == MAX)
        return -1;
   
    id_camion[0] = a.liste_camion[indice_camion]->id_camion;
    *indice_trajet = position_insertion;
    return  0;
    

}

int recupere_par_id_entrepot(char id_entrepot, int nb_entrepot, entrepot *a)
{
    for(int i = 0; i < nb_entrepot; i++)
        if(a[i].id_entrepot == id_entrepot)
            return i;

    return -1;
}


entrepot *enchere_echange(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe)
{

    for(int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        char entrepot_demande = rv[cpt_requete].id_entrepot;
        int indice_e_demande = recupere_par_id_entrepot(entrepot_demande,nb_entrepot,a);
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        int cout_requete_min = rv[cpt_requete].prix_propose_vente;

        for(int cpt_entrepot = 0; cpt_entrepot < nb_entrepot; cpt_entrepot++)
        {
            char entrepot_offre = a[cpt_entrepot].id_entrepot;
            if(entrepot_demande != entrepot_offre)
            {
                int camion_offre = -1;
                int indice_e_offre = recupere_par_id_entrepot(entrepot_offre,nb_entrepot,a);
                int cout_requete = cout_requete_fin_trajet(rv[cpt_requete],a[indice_e_offre],&camion_offre,graphe);
                if(cout_requete < cout_requete_min)
                {
                    cout_requete_min = cout_requete;
                    indice_e_offre_min = indice_e_offre;
                    indice_c_offre_min = camion_offre;
                    cpt_offre++;
                }
            }    
        }

        if(cpt_offre)
        {
            a[indice_e_offre_min].gain_total -= faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min],rv[cpt_requete].origine,rv[cpt_requete].destination,graphe,1);
            a[indice_e_offre_min].gain_total += cout_requete_min + 1;
            a[indice_e_demande].gain_total -= cout_requete_min + 1;
        }
        else
        {
            int camion_demande = -1;
            int cout_requete = cout_requete_fin_trajet(rv[cpt_requete],a[indice_e_demande],&camion_demande,graphe);
            int indice_c_demande = camion_demande;
            a[indice_e_demande].gain_total -= faire_course(a[indice_e_demande].liste_camion[indice_c_demande],rv[cpt_requete].origine,rv[cpt_requete].destination,graphe,1);
        }
        a[indice_e_demande].gain_total += rv[cpt_requete].gain;
    }

    return a;   
}