#------------------------------------------------------------------------------#
# treeview                                                                     #
#------------------------------------------------------------------------------#
from tkinter import *
from tkinter import ttk
VALUES = [('Josef', 'Novák', 'Vanilková'),
          ('Anna', 'Polní', 'Jahodová'),
          ('Jan', 'Černý', 'Citronová'),
          ('Eliška', 'Novotná', 'Vanilková'),
          ('Tomáš', 'Dvořák', 'Čokoládová')]

class myApp:
  def __init__(self, window):
    self.tree = ttk.Treeview(window, columns=('first_name','last_name','ice_cream'), show='headings')

    self.tree.heading('first_name', text='Jméno')
    self.tree.heading('last_name', text='Příjmení')
    self.tree.heading('ice_cream', text='Oblíbená zmrzlina')
    
    for v in VALUES:
      self.tree.insert('', END, values=v)

    self.tree.grid(row=0, column=0, sticky=NSEW)

    self.scrollbar = ttk.Scrollbar(window, orient=VERTICAL, command=self.tree.yview)
    self.tree.configure(yscroll=self.scrollbar.set)
    self.scrollbar.grid(row=0, column=1, sticky=NS)

root = Tk()
app = myApp(root)
root.mainloop()
#------------------------------------------------------------------------------#
