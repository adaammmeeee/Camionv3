from multiprocessing.connection import wait
from operator import length_hint
from select import select
from tkinter import *
import tkinter
from turtle import distance
from unicodedata import digit
import tkintermapview
from tkinter import messagebox
from matplotlib import pyplot as plt
from dataclasses import dataclass
import time
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
    nombre_camion = a[numero].nb_camion
    entrepot = Tk()
    x = 300
    y = 16*nombre_camion + 22
    entrepot.configure(width = x, height = y)
    entrepot.title("Entrepot " + str(numero))
    largeur_liste = 17

    # Création du canvas
    cnv=Canvas(entrepot, width=x, height=y, bg = "white")
    cnv.pack()

    # Création de la liste de camion

    liste = Listbox(entrepot, width=largeur_liste, height=nombre_camion)
    for i in range(nombre_camion):
        liste.insert(i, "Camion " + str(i))
    liste.place(x=0, y=0)
   

    def affiche_trajet():
        try:
            global ligne_trajet
            cnv.delete("all")
            selection = liste.selection_get()
            selection = digit_in_str(selection)
            trajet = a[numero].liste_camion[selection].trajet
            affichage = ""
            for i in range(len(trajet)-1):
                affichage = affichage + str(trajet[i]) + "->"
            affichage = affichage + str(trajet[len(trajet)-1])
            affichage = affichage + "\n"
            affichage = affichage + "Distance parcourue : " + str(a[numero].liste_camion[selection].kilometrage)
            affichage = affichage + "km\n"
            affichage = affichage + "Charge : " + str(a[numero].liste_camion[selection].charge)

            cnv.create_text(largeur_liste +185, 25, text= affichage , fill="black", font=("Arial",10), justify= 'left', )
            try: 
                ligne_trajet.delete()
            except:
                pass
            ligne_trajet =  map_widget.set_path([marker_list[trajet[i]].position for i in range(len(trajet))], color='red')
            def clear_trajet():
                ligne_trajet.delete()
                entrepot.destroy()
            entrepot.protocol("WM_DELETE_WINDOW", clear_trajet)
        except:
           print("Erreur")



    but = Button(entrepot, text="Voir le trajet", command=affiche_trajet, width=largeur_liste-3)
    but.place(x=0, y=16*nombre_camion+2)



    



def click_sur_icone_entrepot(marker):
    values = ['Voir requêtes', 'Voir trajet']
    gestionnaire_entrepot(digit_in_str(marker.text))



# Lecture du fichier pour récupérer la position de tous les agents qu'on stockera dans la variable position
positions = []
f = open("position_sites.csv", "r")
lignes = f.readlines()
for line in lignes:
    # format du tuple : (latitude,longitude)
    tuple = line.partition(";")
    positions.append(tuple)

# Création d'une fenêtre Tkinter
root_tk = tkinter.Tk()
root_tk.geometry(f"{1000}x{650}")
root_tk.title("Camion")


# Création de la map
map_widget = tkintermapview.TkinterMapView(
    root_tk, width=900, height=650, corner_radius=0)
map_widget.place(relx=0.57, rely=0.5, anchor=tkinter.CENTER)
map_widget.set_position(46.8734246, 2.6397020)  # Paris, France
map_widget.set_zoom(6.1)


# Placement de tous les entrepots sur la carte
cpt = 0
marker_list = []
for position in positions:  
    marker = map_widget.set_marker(float(position[0]), float(position[2]), command=click_sur_icone_entrepot, text="Entrepot " + str(
        cpt), text_color="black", marker_color_outside="#A9A9A9", marker_color_circle="#808080", font=10)
    marker_list.append(marker)
    marker = None
    cpt += 1


# Création de la liste d'entrepots
liste = Listbox(root_tk, width=20, height=int(650/16))
for i in range(len(positions)):
    liste.insert(i, "Entrepot " + str(i))
    liste.place(x=0, y=0)

global old
def items_selected(event):
    global old
    # get selected indices
    selected_indice = digit_in_str(str(liste.curselection()))
    marker_list[selected_indice] = map_widget.set_marker(float(positions[selected_indice][0]), float(positions[selected_indice][2]), command=click_sur_icone_entrepot, text="Entrepot " + str(
        selected_indice), text_color="black", marker_color_outside="red", marker_color_circle="red", font=10)

    try:
        marker_list[old] = map_widget.set_marker(float(positions[old][0]), float(positions[old][2]), command=click_sur_icone_entrepot, text="Entrepot " + str(
            old), text_color="black", marker_color_outside="#A9A9A9", marker_color_circle="#808080", font=10)
    except:
        pass

    old = selected_indice
    

    
    
liste.bind('<<ListboxSelect>>', items_selected)


root_tk.mainloop()
