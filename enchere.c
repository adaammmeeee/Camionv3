#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "enchere.h"

requete copie_requete(requete r, float prix_propose)
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

entrepot *enchere_echange_fin(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, float **graphe)
{
    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete].id_entrepot;
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        float cout_requete_min = rv[cpt_requete].prix_propose_vente;

        for (int cpt_entrepot = 0; cpt_entrepot < nb_entrepot; cpt_entrepot++)
        {
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                float cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_offre], &camion_offre, graphe);
                if(camion_offre == -1 && cout_requete)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }

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
            int taille_trajet = a[indice_e_offre_min].liste_camion[indice_c_offre_min]->taille_trajet;
            int pos_camion = a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet[taille_trajet - 1];

            a[indice_e_offre_min].gain_total -= faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], pos_camion, rv[cpt_requete].origine, graphe, 0);
            a[indice_e_offre_min].gain_total -= faire_course(a[indice_e_offre_min].liste_camion[indice_c_offre_min], rv[cpt_requete].origine, rv[cpt_requete].destination, graphe, 1);
            a[indice_e_offre_min].gain_total += cout_requete_min + 1;
            a[indice_e_demande].gain_total -= cout_requete_min + 1;

            printf("ENCHERES : La requete %d->%d de l'acteur %d a été vendue à l'acteur %d au prix de %.2f, il la fera avec le camion %d\n", 
                rv[cpt_requete].origine, rv[cpt_requete].destination, indice_e_demande, indice_e_offre_min, cout_requete_min + 1, indice_c_offre_min);
        }
        else
        {
            int camion_demande = -1;
            int cout_requete = cout_requete_fin_trajet(rv[cpt_requete], a[indice_e_demande], &camion_demande, graphe);
            if(camion_demande == -1 && cout_requete)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if(camion_demande != -1 && cout_requete)
            {
                int indice_c_demande = camion_demande;
                int taille_trajet = a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet;
                int pos_camion = a[indice_e_demande].liste_camion[indice_c_demande]->trajet[taille_trajet - 1];

                a[indice_e_demande].gain_total -= faire_course(a[indice_e_demande].liste_camion[indice_c_demande], pos_camion, rv[cpt_requete].origine, graphe, 0);
                a[indice_e_demande].gain_total -= faire_course(a[indice_e_demande].liste_camion[indice_c_demande], rv[cpt_requete].origine, rv[cpt_requete].destination, graphe, 1);
                printf("ENCHERES : La requete %d->%d de l'acteur %d n'a pas été vendue, il la fera avec le camion %d\n", 
                rv[cpt_requete].origine, rv[cpt_requete].destination, indice_e_demande, indice_c_demande);
            }
            else if(!cout_requete)
            {
                a[indice_e_demande].gain_total -= rv[cpt_requete].gain;
                a[indice_e_demande].gain_total -= rv[cpt_requete].perte;
            }
        }
        a[indice_e_demande].gain_total += rv[cpt_requete].gain;
    }

    return a;
}


entrepot *enchere_echange_insertion(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, float **graphe)
{
    int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
    int taille_new_trajet_min = 0;
    int *new_trajet_min;

    for (int cpt_requete = 0; cpt_requete < nb_requete_vendre; cpt_requete++)
    {
        int indice_e_demande = rv[cpt_requete].id_entrepot;
        int cpt_offre = 0;

        int indice_c_offre_min;
        int indice_e_offre_min;
        float cout_requete_min = rv[cpt_requete].prix_propose_vente;
        
        for (int cpt_entrepot = 0; cpt_entrepot < nb_entrepot; cpt_entrepot++)
        {
            int indice_e_offre = a[cpt_entrepot].id_entrepot;
            if (indice_e_demande != indice_e_offre)
            {
                int camion_offre = -1;
                int taille_new_trajet = 0;
                memset(new_trajet, 0, TAILLE_MAX_TRAJET);
                float cout_requete = insertion(rv[cpt_requete], a[indice_e_offre], &camion_offre, new_trajet, &taille_new_trajet, graphe);
                if((camion_offre == -1 || !taille_new_trajet) && cout_requete)
                {
                    printf("ERREUR : lors du choix du camion faisant le trajet, error in %s\n", __FUNCTION__);
                    return NULL;
                }

                if (cout_requete && cout_requete < cout_requete_min)
                {
                    cout_requete_min = cout_requete;
                    indice_e_offre_min = indice_e_offre;
                    indice_c_offre_min = camion_offre;
                    new_trajet_min = new_trajet;
                    taille_new_trajet_min = taille_new_trajet;
                    cpt_offre++;
                }
            }
        }

        if (cpt_offre)
        {
            a[indice_e_offre_min].liste_camion[indice_c_offre_min]->trajet = new_trajet_min;
            a[indice_e_offre_min].liste_camion[indice_c_offre_min]->taille_trajet = taille_new_trajet_min;

            //il faut pas compter le retour il sera compter apres
            a[indice_e_offre_min].gain_total -= cout_requete_min;
            a[indice_e_offre_min].gain_total += cout_requete_min + 1;
            a[indice_e_demande].gain_total -= cout_requete_min + 1;
        }
        else
        {
            int camion_demande = -1;
            int taille_new_trajet = 0;
            int *new_trajet = calloc(TAILLE_MAX_TRAJET, sizeof(int));
            float cout_requete = insertion(rv[cpt_requete], a[indice_e_demande], &camion_demande, new_trajet, &taille_new_trajet, graphe);
            if(camion_demande == -1 && cout_requete)
            {
                printf("ERREUR : lors du choix du camion faisant le trajet\n");
                return NULL;
            }
            else if(camion_demande != -1 && cout_requete)
            {
                int indice_c_demande = camion_demande;
                a[indice_e_demande].liste_camion[indice_c_demande]->trajet = new_trajet;
                a[indice_e_demande].liste_camion[indice_c_demande]->taille_trajet = taille_new_trajet;

                a[indice_e_demande].gain_total -= cout_requete;
            }
            else if(!cout_requete)
            {
                a[indice_e_demande].gain_total -= rv[cpt_requete].gain;
                a[indice_e_demande].gain_total -= rv[cpt_requete].perte;
            }
        }
        a[indice_e_demande].gain_total += rv[cpt_requete].gain;
    }
    free(new_trajet);

    return a;
}