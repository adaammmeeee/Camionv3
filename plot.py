from matplotlib import pyplot as plt
from dataclasses import dataclass


@dataclass
class Entrepot:
    id : int
    nb_camion : int
    nb_requete : int
    liste_camion : list


def digit_in_str(str):
    string = ""
    for i in str:
        if i.isdigit():
            string = string + i
    return int(string)



# Repartition des acteur
a = []

f = open("gestionnaire", "r")
lignes = f.readlines()

nb_entrepot = digit_in_str(lignes[0])
current = Entrepot(0,0,0,[])
for ligne in lignes:
    if (ligne.startswith("entrepot")):
        current.id = digit_in_str(ligne)

    if (ligne.startswith("nombre de camion")):
        current.nb_camion = digit_in_str(ligne)

    if (ligne.startswith("nombre de requete")):
        current.nb_requete = digit_in_str(ligne)
        a.append(current)
        current = Entrepot(0,0,0,[])
x = []
y = []

for i in range(nb_entrepot):
    x.append(a[i].id)
    y.append(a[i].nb_requete)

plt.bar(x,y)
plt.ylabel('Nombre de requêtes')
plt.xlabel("Entrepot id")
plt.show()

#Brute force vs initialisation classique