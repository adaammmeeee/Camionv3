// Ajoute à la fin du trajet de c une course chargée (ou pas) d'origine jusqu'à destination
// Renvoi le cout de cette course
float faire_course(camion *c, char origine, char destination, float **graphe, int plein);

// Basé sur un algo glouton : voir rapport
entrepot evaluation_meilleure_solution(liste_requete * LR, entrepot a, int nb_requete, float **graphe);

// commentaire à faire
int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, float **graphe);

// Pour chaque trajet de chaque camion de l'entrepot A, on verifie à chaque escale si il est possible d'effectuer la requete r
// Si oui, on simule la requete r et on renvoi le cout de la meilleure simulation.
// Indice trajet indique l'etape à laquelle on à simuler la meilleure insertion
// id_camion est l'id du camion qui effectuera potentiellement la nouvelle insertion
float insertion(int *id_camion, char * new_trajet, entrepot a, requete r, float **graphe);
