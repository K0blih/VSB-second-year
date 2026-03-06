#------------------------------------------------------------------------------#
# treeview, event                                                              #
#------------------------------------------------------------------------------#
from tkinter import *
from tkinter import ttk



class myApp:
  def __init__(self, window):
    self.s = ttk.Style()
    
    self.tree = ttk.Treeview(window, columns=('filename', 'size', 'date_modified'), show='tree headings')
    self.tree.heading('filename', text='Name')
    self.tree.heading('size', text='Size')
    self.tree.heading('date_modified', text='Date modified')
    

    # parent folders
    c = self.tree.insert('', END, values=('C:', '170 GB', '2024/01/19'))
    d = self.tree.insert('', END, values=('D:', '80 GB', '2024/03/07'))
    e = self.tree.insert('', END, values=('E:', '20 KB', '2024/03/05'), open=True)
    
    movies = self.tree.insert(d, END, values=('Movies', '80 GB', '2024/03/07'))
    
    # not-parent folders/files
    self.tree.insert(c, END, values=('System', '150 GB', '2024/01/01'))
    self.tree.insert(c, END, values=('Users', '20 GB', '2024/01/19'))
    self.tree.insert(movies, END, values=('par_parmenu_4K.mkv', '80 GB', '2024/03/07'))
    for _ in range(20):
      self.tree.insert(e, END, values=('obrazek.jpg', '1 KB', '2024/03/05'))
    

    self.tree.grid(row=0, column=0, sticky=NSEW)

    # scrollbar
    self.scrollbar = ttk.Scrollbar(window, 
                                   orient=VERTICAL, 
                                   command=self.tree.yview)
    self.scrollbar.grid(row=0, column=1, sticky=NS)

    self.tree.configure(yscroll=self.scrollbar.set)

    # bind events (selecting in treeview and clicking mouse buttons)
    self.tree.bind('<<TreeviewSelect>>', self.item_selected)
    self.tree.bind('<Button-1>',  self.left_click)
    self.tree.bind('<Button-3>',  self.right_click)

  def item_selected(self, event):
    for selected_item in self.tree.selection():
      print("selected "+str(self.tree.item(selected_item)['values']))
  
  def left_click(self, event):
    print("left click")
  
  def right_click(self, event):
    print("right click position: (%s %s)" % (event.x, event.y))
  
root = Tk()
app = myApp(root)
root.mainloop()
#------------------------------------------------------------------------------#
