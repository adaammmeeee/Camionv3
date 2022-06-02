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
    int id_entrepot;
    int distance_parcouru;

} camion;

typedef struct entrepot
{
    int id_entrepot;
    int nb_requete;
    liste_requete *LR;
    camion *liste_camion;
} entrepot;

void charge_requete(FILE *f, liste_requete *LR);

entrepot *charge_entrepots(char *nomfic, int *nb_entrepot);

void init_liste_requete(liste_requete *LR);

void ajout_requete(liste_requete *LR, char origine, char destination, int gain, int perte);

void liberation(liste_requete *LR);

void affichage(liste_requete *LR);