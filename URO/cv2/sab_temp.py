# -*- coding: utf-8 -*-

from tkinter import *
from math import sqrt
import tkinter.font

class myApp:
    def prevod(self, event=None):
        v = float(self.ent_in.get())

        if self.dir.get() == 0:
            out = (v * (9 / 5)) + 32
            temp_c = v
        else:
            out = (v - 32) * (5 / 9)
            temp_c = out

        self.ent_out.delete(0, END)
        self.ent_out.insert(0, f"{out:.2f}")
        min_c, max_c = -20, 50
        column_top, column_bottom = 80, 292
        x1, x2 = 146, 152

        temp_c = max(min_c, min(temp_c, max_c))
        normalized = (temp_c - min_c) / (max_c - min_c)
        y = column_bottom - (column_bottom - column_top) * normalized
        self.ca.coords(self.thermo_rect, x1, column_bottom, x2, y)


    def __init__(self, root):

        root.title('Převodník teplot')
        root.resizable(False, False)
        root.bind('<Return>', self.prevod)        

        def_font = tkinter.font.nametofont("TkDefaultFont")
        def_font.config(size=16)

        self.left_frame = Frame(root)
        self.right_frame = Frame(root)
        
        self.ent_in_frame = Frame(self.left_frame)
        self.ent_out_frame = Frame(self.left_frame)
        self.radio_frame = Frame(self.left_frame)

        self.dir = IntVar()
        self.dir.set(0)
        self.lbl_radio = Label(self.left_frame, text="Smer prevodu")
        self.radio1 = Radiobutton(self.radio_frame, value=0, variable=self.dir, text="C->F")
        self.radio2 = Radiobutton(self.radio_frame, value=1, variable=self.dir, text="F->C")

        self.lbl_in = Label(self.ent_in_frame, text="Vstup")
        self.ent_in = Entry(self.ent_in_frame, width=10, font = def_font)
        self.ent_in.insert(0, '0')

        self.lbl_out = Label(self.ent_out_frame, text="Vystup")
        self.ent_out = Entry(self.ent_out_frame, width=10, font = def_font)
        self.btn = Button(self.ent_out_frame, text="Convert", command=self.prevod)

        self.ca = Canvas(self.right_frame, width=300, height=400)
        self.photo = PhotoImage(file="th_empty.png")
        self.ca.create_image(150, 200, image=self.photo)
        self.thermo_rect = self.ca.create_rectangle(146, 292, 152, 292, fill='blue', outline='blue')

        self.lbl_login = Label(self.left_frame, text="Richard Chovanec CHO0289")

        self.left_frame.pack(side="left", fill=Y)
        self.right_frame.pack(side="right")
        self.lbl_radio.pack()
        self.radio_frame.pack()
        self.radio1.pack(side="left", padx=5)
        self.radio2.pack(side="left", padx=5)
        self.ent_in_frame.pack()
        self.ent_out_frame.pack()
        self.lbl_in.pack()  
        self.ent_in.pack()
        self.lbl_out.pack()
        self.ent_out.pack()
        self.btn.pack(pady=10)
        self.ca.pack()
        self.lbl_login.pack(side="bottom", padx=10)

        self.ent_in.focus_force()


root = Tk()
app = myApp(root)
root.mainloop()

