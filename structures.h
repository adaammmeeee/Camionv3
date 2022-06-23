#define DISTANCE_MAX 2500000
#define MAX 0x7fffffff                  // Valeur maximale pour une variable de type int
#define NB_MAX_CAMION 256
#define TAILLE_MAX_TRAJET 256

typedef struct requete
{
    int id_entrepot;
    int origine;
    int destination;
    int gain;
    int perte;
    int prix_propose_vente;
    int a_vendre;
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
    int id_camion; //pour l'affichage
    int distance_parcouru;
    int taille_trajet;
    int *trajet;
    int *charge;
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

int cout_distance(int distance);

void affichage_requete(liste_requete *LR, int **graphe);

void affichage_entrepot(entrepot a, int **graphe);

void analyse_donnees(entrepot *a, int nb_entrepot);