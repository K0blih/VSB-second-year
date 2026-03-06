#------------------------------------------------------------------------------#
# notebook                                                              #
#------------------------------------------------------------------------------#
from tkinter import *
from tkinter import ttk

class myApp:
  def __init__(self, window):
    self.nb = ttk.Notebook(window)
    self.nb.pack()

    self.f1 = ttk.Frame(self.nb ,width=400, height=280)
    self.f2 = ttk.Frame(self.nb, width=400, height=280)

    self.nb.add(self.f1, text="Page 1")
    self.nb.add(self.f2, text="Page 2")

    Label(self.f1, text="Prvni stranka").pack()

root = Tk()
app = myApp(root)
root.mainloop()
#------------------------------------------------------------------------------#
