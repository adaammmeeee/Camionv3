#define DISTANCE_MAX 2300
#define MAX 0x7fffffff                  // Valeur maximale pour une variable de type int


typedef struct requete
{
    char id_entrepot;
    char origine;
    char destination;
    int gain;
    int perte;
    int prix_propose_vente;
    struct requete *suiv;
    struct requete *prec;
} requete;

typedef struct liste_requete
{
    struct requete *prem;
    struct requete *dern;
} liste_requete;

typedef struct camion
{
    char id_entrepot;
    char id_camion;
    int distance_parcouru;
    char *trajet;
    char *charge;

} camion;

typedef struct entrepot
{
    int id_entrepot;
    int gain_total;
    int nb_requete;
    int nb_camion;
    liste_requete *LR;
    camion **liste_camion;
} entrepot;

typedef struct enchere
{
    int indice_e_offre;
    int indice_c_offre;
    int prix;
} enchere;

/* 
Initialise à partir du fichier f une requete qui est ajoutée dans la liste chainée LR 
La requête n'est pas ajouté à la fin mais dans l'ordre croissant : gain_requete  - cout_trajet_requete
*/
void charge_requete(FILE *f, liste_requete *LR, int ** graphe, char id_entrepot);

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
void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int ** graphe, char id_entrepot);

/*
Libère LR
*/
void liberation(liste_requete *LR);

// Libère un entrepot
void libere_acteur(entrepot a);

// Affiche une liste de requete LR
void affichage_requete(liste_requete *LR);

// Affiche la structure entrepot
void affichage_entrepot(entrepot a);

// Calcul le coût d'un trajet à partir de la distance d avec une fonction affine
int calcul_cout_trajet(int d);

// Modifie le trajet de c en lui faisant faire la course chargé (ou pas) d'origine jusqu'à destination
// Renvoi le cout de cette course
int faire_course(camion *c, char origine, char destination, int **graphe, int plein);

// Basé sur un algo glouton : voir rapport
entrepot evaluation_meilleure_solution(liste_requete * LR, entrepot a, int nb_requete, int **graphe);

// renvoi un graphe (tableau à double entrée) à partir du fichier
int ** charge_graphe( char *nomfic, int nb_entrepots);

// Renvoi le nombre d'entrepot total
int charge_nombre_entrepots(char * nomfic);

// todo
int cout_requete_fin_trajet(requete nouv, entrepot a, int *indice_camion, int **graphe);

// Tous les camions de l'entrepot a retourne à leurs positions initiales (l'id de l'entrepot a)
entrepot retour_a_la_casa(entrepot a, int ** graphe);

// Renvoi une copie de *r avec le champ prix_propose_vente rempli
requete copie_requete(requete * r, int prix_propose);

// Pour chaque trajet de chaque camion de l'entrepot A, on verifie à chaque escale si il est possible d'effectuer la requete r
// Si oui, on simule la requete r et on renvoi le cout de la meilleure simulation.
// Indice trajet indique l'etape à laquelle on à simuler la meilleure insertion
// id_camion est l'id du camion qui effectuera potentiellement la nouvelle insertion
int insertion(int * indice_trajet, char * id_camion, entrepot a, requete r, int **graphe);

// todo
entrepot *enchere_echange(requete *rv, int nb_requete_vendre, int nb_entrepot, entrepot *a, int **graphe);