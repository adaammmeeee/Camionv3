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

void charge_requete(FILE *f, liste_requete *LR, int ** graphe, char id_entrepot);

entrepot *charge_entrepots(char *nomfic, int ** graphe);

void init_liste_requete(liste_requete *LR);

void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte, int ** graphe, char id_entrepot);

void liberation(liste_requete *LR);

void libere_acteur(entrepot a);

void affichage_requete(liste_requete *LR);

void affichage_entrepot(entrepot a);

int calcul_cout_trajet(int d);

int faire_course(camion *c, char origine, char destination, int **graphe, int plein);

int course_basique(int **graphe, entrepot a);

entrepot evaluation_meilleure_solution(liste_requete * LR, entrepot a, int nb_requete, int **graphe);

int ** charge_graphe( char *nomfic, int nb_entrepots);

int charge_nombre_entrepots(char * nomfic);

void proposer_prix(camion c, int prix, enchere * offre, int nb_offre);

int cout_requete_fin_trajet(requete nouv, entrepot a, char *id_camion, int **graphe);

void retour_a_la_casa(entrepot a, int ** graphe);

requete copie_requete(requete * r, int prix_propose);

int test_insertion(entrepot a, requete r, int ** graphe);

int insertion(int * indice_trajet, char * id_camion, entrepot a, requete r, int **graphe);
