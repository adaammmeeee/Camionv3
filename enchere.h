#include "chemins_gloutons.h"

// Renvoi une copie de *r avec le champ prix_propose_vente rempli
requete **mise_en_vente(entrepot *a, int nb_entrepot, int *nb_requete_vente);

// commentaire à faire
entrepot *enchere_echange_fin(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe);

entrepot *enchere_echange_insertion(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe);

entrepot *confiance(requete **rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe);