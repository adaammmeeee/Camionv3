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

valeur0 = [] # sans enchere
valeur1 = [] # appel de confiance
valeur2 = [] # enchere

f = open("analyse0", "r")
lignes = f.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur0.append(float(tuple[2]))
f.close()

f = open("analyse2", "r")
lignes = f.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur1.append(float(tuple[2]))
f.close()

f = open("analyse1", "r")
lignes = f.readlines()
for ligne in lignes:
    tuple = ligne.partition(":")
    valeur2.append(float(tuple[2]))
f.close()

x = [i for i in range(len(valeur0))]

tri_table(valeur1, valeur2, valeur0)

plt.plot(x, valeur0, label="Témoin")
plt.plot(x, valeur1, label="Enchère")
plt.plot(x, valeur2, label="Appel de confiance")
plt.legend()

plt.show()
