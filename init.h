/* 
Initialise à partir du fichier f une requete qui est ajoutée dans la liste chainée LR 
La requête n'est pas ajouté à la fin mais dans l'ordre croissant : gain_requete  - cout_trajet_requete
*/
int charge_requete(FILE *f, liste_requete *LR, int **graphe, char id_entrepot);

/*
Renvoi un tableau d'entrepot à partir du fichier nomfic
*/
entrepot *charge_entrepots(char *nomfic, int ** graphe);

/*
initialise une liste chainée de requete
*/
void init_liste_requete(liste_requete *LR);

/*
Ajoute une requete à LR toujours dans l'ordre croissant : gain_requete - cout_trajet_requete
*/
int ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int **graphe, char id_entrepot);

/*
Libère LR
*/
void liberation(liste_requete *LR);

// Libère un entrepot
void libere_acteur(entrepot a);

// renvoi un graphe (tableau à double entrée) à partir du fichier
int ** charge_graphe( char *nomfic, int nb_entrepots);

// Renvoi le nombre d'entrepot total
int charge_nombre_entrepots(char * nomfic);