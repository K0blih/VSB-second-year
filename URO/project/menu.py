import tkinter as tk


class MenuBar:
    def __init__(self, root, on_action):
        self.root = root
        self.on_action = on_action
        self.menu = tk.Menu(self.root)
        self._build()

    def _build(self):
        menu_soubor = tk.Menu(self.menu, tearoff=0)
        menu_soubor.add_command(label="Nová položka", command=lambda: self.on_action("new"))
        menu_soubor.add_command(label="Otevřít", command=lambda: self.on_action("open"))
        menu_soubor.add_command(label="Uložit", command=lambda: self.on_action("save"))
        menu_soubor.add_command(label="Export", command=lambda: self.on_action("export"))
        menu_soubor.add_separator()
        menu_soubor.add_command(label="Konec", command=self.root.quit)
        self.menu.add_cascade(label="Soubor", menu=menu_soubor)

        menu_upravy = tk.Menu(self.menu, tearoff=0)
        menu_upravy.add_command(label="Smazat", command=lambda: self.on_action("delete"))
        self.menu.add_cascade(label="Upravit", menu=menu_upravy)

        menu_zobrazeni = tk.Menu(self.menu, tearoff=0)
        menu_zobrazeni.add_command(label="Seznam", command=lambda: self.on_action("view_list"))
        menu_zobrazeni.add_command(label="Detail položky", command=lambda: self.on_action("view_detail"))
        self.menu.add_cascade(label="Zobrazení", menu=menu_zobrazeni)

        menu_statistiky = tk.Menu(self.menu, tearoff=0)
        menu_statistiky.add_command(label="Přehled", command=lambda: self.on_action("stats_overview"))
        menu_statistiky.add_command(label="Podle žánrů", command=lambda: self.on_action("stats_by_genre"))
        self.menu.add_cascade(label="Statistiky", menu=menu_statistiky)

        menu_napoveda = tk.Menu(self.menu, tearoff=0)
        menu_napoveda.add_command(label="O aplikaci", command=lambda: self.on_action("about"))
        self.menu.add_cascade(label="Nápověda", menu=menu_napoveda)
