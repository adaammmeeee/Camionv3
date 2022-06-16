#include "chemins_gloutons.h"

// Renvoi une copie de *r avec le champ prix_propose_vente rempli
requete copie_requete(requete r, float prix_propose);

// commentaire à faire
entrepot *enchere_echange(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, float **graphe);
