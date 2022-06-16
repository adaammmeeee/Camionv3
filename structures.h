#define DISTANCE_MAX 1000
#define MAX 0x7fffffff                  // Valeur maximale pour une variable de type int
#define NB_MAX_CAMION 64
#define TAILLE_MAX_TRAJET 64
#define cout_distance(d) d?1.1 * d + 50:0

typedef struct requete
{
    int id_entrepot;
    int origine;
    int destination;
    float gain;
    float perte;
    float prix_propose_vente;
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
    float distance_parcouru;
    int taille_trajet;
    int *trajet;
    int *charge;
} camion;

typedef struct entrepot
{
    int id_entrepot;
    float gain_total;
    int nb_requete;
    int nb_camion;
    liste_requete *LR;
    camion **liste_camion;
} entrepot;

typedef struct enchere
{
    int indice_e_offre;
    int indice_c_offre;
    float prix;
} enchere;
