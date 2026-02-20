from tkinter import *

class MyApp:
    def __init__(self, master):
        self.bu1 = Button(master, text="Button 1", command = root.destroy)
        self.bu2 = Button(master, text="Button 2", command = root.destroy)
        self.bu3 = Button(master, text="Button 3", command = root.destroy)
        self.bu4 = Button(master, text="Button 4", command = root.destroy)

        self.bu1.pack(side="top")
        self.bu2.pack(side="bottom")
        self.bu3.pack(side="left")
        self.bu4.pack(side="right")

root = Tk()
app = MyApp(root)
root.title("Hello World")
root.mainloop()