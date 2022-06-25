from tkinter import *


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


def create_circle(x,y,r,canvasName,identrepot):
    canvasName.create_oval(x-r,y-r,x+r,y+r,fill="#C5C600" )
    w.create_text(x, y, text=identrepot, fill="black", font=('Helvetica'+ str(int(r/2))+'bold'))




hauteur = 720
largeur = 1280
hauteur_case = 16
case_visible = 40

master = Tk()
master.title("Camion")


w = Canvas(master,
           width=largeur,
           height=hauteur,)
w.pack(expand=YES, fill=BOTH)


create_circle(largeur/2,hauteur/2,10, w,"1")



# liste
liste = Listbox(master,width = 20, height=40)
for i in range(96):
    liste.insert(i, "Entrepot " + str(i))



liste.place(x=0, y=0)

but = Button(master, text="Check request", command=check_list)
but.place(x=0, y=hauteur_case*case_visible +2)


master.mainloop()