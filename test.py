from tkinter import *
import tkinter
from turtle import distance
from unicodedata import digit
import tkintermapview
from tkinter import messagebox
from matplotlib import pyplot as plt
from dataclasses import dataclass
import re
import string 

@dataclass
class Camion:
    id_camion : int
    kilometrage : float
    trajet : list
    charge : list


@dataclass
class Entrepot:
    id : int
    nb_camion : int
    liste_camion : list


def digit_in_str(str):
    string = ""
    for i in str:
        if i.isdigit():
            string = string + i
    return int(string)

def float_in_str(str):
    string = ""
    for i in str:
        if i.isdigit() or i == ".":
            string = string + i
    return float(string)


def numbers_in_str(str):
    numbers = []
    number = ""
    for i in str:
        if i.isdigit():
            number = number + i
        else:
            if number != "":
                numbers.append(int(number))
                number = ""
    
    if (number != ""):
        numbers.append(int(number))
    return numbers



def extraction_trajet():
    a = []
    with open("trajet", "r") as f:
       
        while True:
            current_a = Entrepot(0,0,[])
            line = f.readline()
            if not line :
                break
            current_a.id = digit_in_str(line)
            line = f.readline()
            current_a.nb_camion = digit_in_str(line)
            for i in range(current_a.nb_camion):
                c = Camion(0,0,[],[])
                #Lecture de l'id camion
                line = f.readline()
                c.id_camion = digit_in_str(line)
                #Lecture de la distance parcouru
                line = f.readline()
                c.kilometrage = float_in_str(line)
                #Lecture du trajet
                line = f.readline()
                c.trajet = numbers_in_str(line)
                #Lecture de la charge
                line = f.readline()
                c.charge = numbers_in_str(line)
                current_a.liste_camion.append(c)
            a.append(current_a)


    

    return a


def gestionnaire_entrepot(numero):
    # Initialisation des entrepots
    a = extraction_trajet()
    entrepot = tkinter.Tk()
    x = 300
    y = 300
    entrepot.geometry(f"{x}x{y}")
    entrepot.title("Entrepot " + str(numero))
    # Création de la liste
    nombre_camion = 8
    liste = Listbox(entrepot, width=0, height=nombre_camion)
    for i in range(nombre_camion):
        liste.insert(i, "Camion " + str(i))
    liste.place(x=0, y=0)
    cnv=Canvas(entrepot, width=x-30, height=y, bg="ivory")
    cnv.pack(padx=55, pady=0)

    def affiche_trajet():
        try:
            selection = liste.selection_get()
            selection = digit_in_str(selection)
            print(a[numero])

        except:
            print("Aucune requête n'a été sélectionnée")

    but = Button(entrepot, text="Voir le trajet", command=affiche_trajet)
    but.place(x=0, y=16*nombre_camion+2)


def click_airport_marker_event(marker):
    values = ['Voir requêtes', 'Voir trajet']
    gestionnaire_entrepot(digit_in_str(marker.text))


# Création d'une fenêtre Tkinter
root_tk = tkinter.Tk()
root_tk.geometry(f"{700}x{500}")
root_tk.title("map_view_example.py")


# Création de la map
map_widget = tkintermapview.TkinterMapView(
    root_tk, width=800, height=600, corner_radius=0)
map_widget.place(relx=0.5, rely=0.5, anchor=tkinter.CENTER)
map_widget.set_position(46.8734246, 2.6397020)  # Paris, France
map_widget.set_zoom(6.1)


# Lecture du fichier pour récupérer la position de tous les agents qu'on stockera dans la variable position
positions = []
f = open("position_sites.csv", "r")
lignes = f.readlines()
for line in lignes:
    # format du tuple : (latitude,longitude)
    tuple = line.partition(";")
    positions.append(tuple)



# Placement de tous les entrepots sur la carte
cpt = 0
marker_list = []
for position in positions:
    marker = map_widget.set_marker(float(position[0]), float(position[2]), command=click_airport_marker_event, text="Entrepot " + str(
        cpt), text_color="black", marker_color_outside="#A9A9A9", marker_color_circle="#808080", font=10)
    marker_list.append(marker)
    cpt += 1

root_tk.mainloop()
