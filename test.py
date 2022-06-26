from tkinter import *
import tkinter
import tkintermapview
from tkinter import messagebox
from matplotlib import pyplot as plt


def gestionnaire_entrepot(numero):
    entrepot = tkinter.Tk()
    entrepot.geometry(f"{300}x{300}")
    entrepot.title("Gestionnaire de l'entrepot " + numero)
    # Création de la liste
    nombre_camion = 8
    liste = Listbox(entrepot, width=30, height=nombre_camion)
    for i in range(nombre_camion):
        liste.insert(i, "Camion " + str(i))
    liste.place(x=0, y=0)

    def check_list():
        try:
            s = liste.selection_get()
            print(s)
        except:
            print("Aucune requête n'a été sélectionnée")

    but = Button(entrepot, text="Voir le trajet", command=check_list)
    but.place(x=0, y=16*nombre_camion+2)


def click_airport_marker_event(marker):
    values = ['Voir requêtes', 'Voir trajet']
    gestionnaire_entrepot(marker.text[-1])


# Création d'une fenêtre Tkinter
root_tk = tkinter.Tk()
root_tk.geometry(f"{800}x{600}")
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


"""
#Definition des proportions de la fenetre
hauteur = 2000
largeur = 2000
hauteur_case = 16
case_visible = 40

#Création de la fenêtre
master = Tk()
master.title("Camion")
w = Canvas(master, width=largeur, height=hauteur,)
w.pack(expand=YES, fill=BOTH)


    
plt.scatter(x=df['Longitude'], y=df['Latitude'])
plt.show()



#Création de la liste
liste = Listbox(master, width=20, height=40)
for i in range(96):
    liste.insert(i, "Entrepot " + str(i))


liste.place(x=0, y=0)

but = Button(master, text="Check request", command=check_list)
but.place(x=0, y=hauteur_case*case_visible + 2)


master.mainloop()
"""
