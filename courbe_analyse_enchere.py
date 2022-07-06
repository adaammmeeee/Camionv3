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



for i in range(nb_entrepot):
    x.append(a[i].id)

valeur1 = [] # sans enchere
valeur2 = [] # avec enchere
valeur3 = [] # appel de confiance

stats1 = [] # sans enchere
stats2 = [] # avec enchere
stats3 = [] # appel de confiance


f = open("analyse1", "r")
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

f = open("analyse3", "r")
lignes = f.readlines()
for i in range(len(lignes)):
    tuple = lignes[i].partition(":")
    if (i>=(len(lignes)-4)):
        stats3.append(float(tuple[2]))
    else:
        valeur3.append(float(tuple[2]))
f.close()


df = pd.DataFrame({'sans enchere': valeur1,
                   'enchere': valeur2,
                   'appel de confiance': valeur3}
                   , index=x)
ax = df.plot.bar(rot=0)

x = ["moyenne", "max", "min", "ecart-type"]


plt.subplot(212)

df = pd.DataFrame({'sans enchere': stats1,
                   'enchere': stats2,
                   'appel de confiance': stats3}
                   , index=x)
ax = df.plot.bar(rot=0)

plt.show()
