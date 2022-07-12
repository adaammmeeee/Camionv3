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


moyenne0 = []  # sans enchere
moyenne1 = []  # appel de confiance
moyenne2 = []  # enchere
ecartype0 = []  # sans enchere
ecartype1 = []  # appel de confiance
ecartype2 = []  # enchere

f = open("analyse0", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    moyenne0.append(float(tuple[2]))
    ecartype0.append(float(tuple1[2]))
f.close()

f = open("analyse2", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    moyenne1.append(float(tuple[2]))
    ecartype1.append(float(tuple1[2]))
f.close()

f = open("analyse1", "r")
lignes = f.readlines()
for i in range(0, len(lignes), 2):
    tuple = lignes[i].partition(":")
    tuple1 = lignes[i+1].partition(":")
    moyenne2.append(float(tuple[2]))
    ecartype2.append(float(tuple1[2]))
f.close()

x = [i for i in range(len(moyenne0))]

tri_table(moyenne1, moyenne2, moyenne0)

#plt.errorbar(x, moyenne0, ecartype0, linestyle='None', marker='^')
plt.plot(x, moyenne0, label="Sans échange")
plt.plot(x, moyenne1, label="Enchère")
plt.plot(x, moyenne2, label="Appel de confiance")
plt.legend()

plt.show()


