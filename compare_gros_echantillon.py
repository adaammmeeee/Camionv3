from calendar import MONDAY
from unittest import result
from matplotlib import pyplot as plt
import pandas as pd
from dataclasses import dataclass


def tri_table(table_tri, table1, table2):
    for i in range(len(table_tri)):
        for j in range(len(table_tri)):
            if table_tri[i] < table_tri[j]:
                buffer = table_tri[i]
                table_tri[i] = table_tri[j]
                table_tri[j] = buffer
                buffer = table1[i]
                table1[i] = table1[j]
                table1[j] = buffer
                buffer = table2[i]
                table2[i] = table2[j]
                table2[j] = buffer


liste_moyenne0 = []  # sans enchere
liste_moyenne1 = []  # appel de confiance
liste_moyenne2 = []  # enchere
ecartype0 = []  # sans enchere
ecartype1 = []  # appel de confiance
ecartype2 = []  # enchere

f = open("analyse0", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    liste_moyenne0.append(float(tuple[2]))
    ecartype0.append(float(tuple1[2]))
f.close()

f = open("analyse2", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    liste_moyenne2.append(float(tuple[2]))
    ecartype2.append(float(tuple1[2]))
f.close()

f = open("analyse1", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    liste_moyenne1.append(float(tuple[2]))
    ecartype1.append(float(tuple1[2]))
f.close()

x = [i for i in range(len(liste_moyenne0))]

tri_table(liste_moyenne2, liste_moyenne1, liste_moyenne0)
moyenne0 =  round(sum(liste_moyenne0)/len(liste_moyenne0),2)
moyenne1 =  round(sum(liste_moyenne1)/len(liste_moyenne1),2)
moyenne2 =  round(sum(liste_moyenne2)/len(liste_moyenne2),2)

#plt.errorbar(x, liste_moyenne0, ecartype0, linestyle='None', marker='^')
plt.plot(x, liste_moyenne0, label="Sans échanges (moyenne = "+str(moyenne0)+ ")")
plt.plot(x, liste_moyenne1, label="Appel de confiance (moyenne = "+str(moyenne1) + ")")
plt.plot(x, liste_moyenne2, label="Enchères (moyenne = "+str(moyenne2) + ")")
plt.legend()
plt.ylabel("Bénéfice moyen d'une itération (90 acteurs)")
plt.title("Comparaison différents type d'échanges sur 100 itérations")
plt.savefig("comparaison_echange_benefice.svg")
plt.show()

ecartypem0 = round(sum(ecartype0)/len(ecartype0),2)
ecartypem1 = round(sum(ecartype1)/len(ecartype1),2)
ecartypem2 = round(sum(ecartype2)/len(ecartype2),2)

tri_table(ecartype2, ecartype1, ecartype0)

plt.ylabel("Ecart-type d'une itération (90 acteurs)")
plt.title("Comparaison différents type d'échanges sur 100 itérations")
plt.plot(x, ecartype0, label="Sans échanges (moyenne = "+str(ecartypem0)+ ")")
plt.plot(x, ecartype1, label="Appel de confiance (moyenne = "+str(ecartypem1) + ")")
plt.plot(x, ecartype2, label="Enchères (moyenne = "+str(ecartypem2) + ")")
plt.legend()
plt.savefig("comparaison_echange_ecart_type.svg")
plt.show()
