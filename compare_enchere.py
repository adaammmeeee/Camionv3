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

def tri_entrepot(entrepot):
    for i in range(len(entrepot)):
        for j in range(len(entrepot)):
            if entrepot[i].nb_requete < entrepot[j].nb_requete:
                buffer = entrepot[i]
                entrepot[i] = entrepot[j]
                entrepot[j] = buffer

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

tri_entrepot(a)

x = []
y = []


for i in range(nb_entrepot):
    x.append(i)
    y.append(a[i].nb_requete)

plt.plot(x, y)
plt.title("Nombre de requete par entrepot")
plt.savefig("repartition_test_enchere.svg")

plt.legend()

"""
valeur1 = [] # sans enchere
valeur2 = [] # avec enchere
valeur3 = [] # appel de confiance

stats1 = [] # sans enchere
stats2 = [] # avec enchere
stats3 = [] # appel de confiance

f = open("analyse0", "r")
lignes = f.readlines()
for i in range(len(lignes)):
    tuple = lignes[i].partition(":")
    if (i>=(len(lignes)-4)):
        stats1.append(float(tuple[2]))
    else:
        valeur1.append(float(tuple[2]))
f.close()

f = open("analyse2", "r")
lignes = f.readlines()
for i in range(len(lignes)):
    tuple = lignes[i].partition(":")
    if (i>=(len(lignes)-4)):
        stats2.append(float(tuple[2]))
    else:
        valeur2.append(float(tuple[2]))
f.close()

f = open("analyse1", "r")
lignes = f.readlines()
for i in range(len(lignes)):
    tuple = lignes[i].partition(":")
    if (i>=(len(lignes)-4)):
        stats3.append(float(tuple[2]))
    else:
        valeur3.append(float(tuple[2]))
f.close()

"""

plt.show()
