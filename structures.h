#define DISTANCE_MAX 2300
#define MAX 0x7fffffff                  // Valeur maximale pour une variable de type int
#define NB_MAX_CAMION 64
#define TAILLE_MAX_TRAJET 64
#define cout_distance(d) 0.8*d+20

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
    int distance_parcouru;
    char *trajet;
    char *charge;

} camion;

typedef struct entrepot
{
    char id_entrepot;
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
