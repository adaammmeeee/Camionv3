#define DISTANCE_MAX 2300

typedef struct requete
{
    char origine;
    char destination;
    int gain;
    int perte;
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
    int cout_total;
    int nb_requete;
    int nb_camion;
    liste_requete *LR;
    camion **liste_camion;
} entrepot;

typedef struct enchere
{
    camion* camion_gerant;
    int prix;
} enchere;

void charge_requete(FILE *f, liste_requete *LR, int ** graphe);

entrepot *charge_entrepots(char *nomfic, int ** graphe);

void init_liste_requete(liste_requete *LR);

void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int ** graphe);

void liberation(liste_requete *LR);

void libere_acteur(entrepot a);

void affichage_requete(liste_requete *LR);

void affichage_entrepot(entrepot a);

int calcul_cout_trajet(int d);

int faire_course(camion *c, char origine, char destination, int **graphe);

int course_basique(int **graphe, entrepot a);

int evaluation_meilleure_solution(liste_requete * LR, entrepot a, int nb_requete, int **graphe);

int ** charge_graphe( char *nomfic, int nb_entrepots);

int charge_nombre_entrepots(char * nomfic);

void proposer_prix(camion *c, int prix, enchere * offre, int nb_offre);

int cout_requete_fin_trajet(requete *nouv, entrepot a, int **graphe);

