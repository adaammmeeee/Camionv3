#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structures.h"
#include "enchere.h"

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
