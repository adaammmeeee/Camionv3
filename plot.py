from unittest import result
from matplotlib import pyplot as plt
import pandas as pd
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

f.close()

x = []
y = []


for i in range(nb_entrepot):
    x.append(a[i].id)
    y.append(a[i].nb_requete)
"""
plt.bar(x,y)
plt.ylabel('Nombre de requêtes')
plt.xlabel("Entrepot id")

"""
#Brute force vs initialisation classique



valeur_brute = []
valeur_insertion = []
valeur_insertion_fin = []

f1 = open("resultat_brute", "r")
lignes = f1.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur_brute.append(float(tuple[2]))
f1.close()

f2 = open("resultat_insertion_fin", "r")
lignes = f2.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur_insertion_fin.append(float(tuple[2]))
f2.close()

f3 = open("resultat_insertion", "r")
lignes = f3.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur_insertion.append(float(tuple[2]))
f3.close()

print(valeur_insertion)


df = pd.DataFrame({'insertion': valeur_insertion,
                   'insertion_fin': valeur_insertion_fin,
                   'brute force': valeur_brute}
                   , index=x)
ax = df.plot.bar(rot=0)

plt.show()