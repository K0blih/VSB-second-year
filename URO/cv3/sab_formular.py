# -*- coding: utf-8 -*-

from tkinter import *
from tkinter import ttk
import MultiListbox as table


data = [
       ["Petr", "Bílý","045214/1512", "17. Listopadu", 15, "Ostrava", 70800,"poznamka"],
       ["Jana", "Zelený","901121/7238", "Vozovna", 54, "Poruba", 78511,""],
       ["Karel", "Modrý","800524/5417", "Porubská", 7, "Praha", 11150,""],
       ["Martin", "Stříbrný","790407/3652", "Sokolovská", 247, "Brno", 54788,"nic"]]


class App:

    def __init__(self, root):
        self.row = IntVar()
        self.jmeno = StringVar()
        self.prijmeni = StringVar()
        self.rc = StringVar()
        self.ulice = StringVar()
        self.cp = StringVar()
        self.mesto = StringVar()
        self.psc = StringVar()


        self.mlb = table.MultiListbox(root, (('Jméno', 20), ('Příjmení', 20), ('Rodné číslo', 12)))
        for i in range(len(data)):
            self.mlb.insert(END, (data[i][0], data[i][1],data[i][2]))
        self.mlb.pack(expand=YES,fill=BOTH, padx=10, pady=10)
        self.mlb.subscribe( lambda row: self.edit( row ) )


        self.nb = ttk.Notebook(root)
        self.p1 = Frame(self.nb)
        self.p2 = Frame(self.nb)
        
        self.nb.pack(fill=BOTH, padx=5, pady=5)

        text=Text(self.p2, height=5, width=20)
        text.pack(expand=1,fill=BOTH)

        self.menubar = Menu(root)
    
        self.filemenu = Menu(self.menubar, tearoff=0)
        self.filemenu.add_command(label="Konec", command=root.quit)
        self.menubar.add_cascade(label="Soubor", menu=self.filemenu)

        root.config(menu=self.menubar)      

    def edit(self, row):
        self.row=row
        print (data[row])
        self.jmeno.set(data[row][0])
             

root = Tk()
root.wm_title("Formulář")
app = App(root)
root.mainloop()

