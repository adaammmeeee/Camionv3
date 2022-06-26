import tkinter
import tkintermapview
from matplotlib import pyplot as plt


def test():
    print("Coucou")

def check_list():
    try:
        s = liste.selection_get()
        requete = Tk()
        requete.title("Requête de l'" + s)
        requete.geometry("300x300")
        label_title = Label(requete, text="Requête")
        label_title.pack()
    except:
        print('no selection')


def create_circle(x, y, r, canvasName, identrepot):
    canvasName.create_oval(x-r, y-r, x+r, y+r, fill="#C5C600")
    w.create_text(x, y, text=identrepot, fill="black",
                  font=('Helvetica' + str(int(r/2))+'bold'))


# create tkinter window
root_tk = tkinter.Tk()
root_tk.geometry(f"{800}x{600}")
root_tk.title("map_view_example.py")

# create map widget
map_widget = tkintermapview.TkinterMapView(root_tk, width=800, height=600, corner_radius=0)
map_widget.place(relx=0.5, rely=0.5, anchor=tkinter.CENTER)
map_widget.set_position(46.8734246,2.6397020)  # Paris, France
map_widget.set_zoom(6.1)

#Lecture du fichier pour récupérer la position de tous les agents qu'on stockera dans la variable position
positions = []
f = open("position_sites.csv", "r")
lignes = f.readlines()
for line in lignes:
    # format du tuple : (latitude,longitude)
    tuple = line.partition(";")
    positions.append(tuple)

cpt = 0
marker_list = []
for position in positions:
    marker = map_widget.set_marker(float(position[0]), float(position[2]), text="Entrepot " + str(cpt),marker_color_outside = "#A9A9A9", marker_color_circle = "#808080", font = 10)
    marker_list.append(marker)
    cpt += 1
    
path_1 = map_widget.set_path([marker_list[0].position, marker_list[1].position], color="#FF0000", command = test())

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

#On a chargé un tuple contenant nos positions maintenant on va créer nos entrepots aux positions de ces tuples 
i = 0
for position in positions:
    x = (float(position[0]) - 40) * ((largeur)/10)-150 #à expliquer
    y = (float(position[2])+3) * (hauteur/11) 
    create_circle(x, y, 10, w, str(i))
    i += 1

    



#Création de la liste
liste = Listbox(master, width=20, height=40)
for i in range(96):
    liste.insert(i, "Entrepot " + str(i))


liste.place(x=0, y=0)

but = Button(master, text="Check request", command=check_list)
but.place(x=0, y=hauteur_case*case_visible + 2)


master.mainloop()
"""