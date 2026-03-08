from tkinter import ttk


class TreeviewPanel(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)

        self.column_layout = [
            ("nazev", 4, 150),
            ("typ", 2, 90),
            ("zanr", 2, 100),
            ("stav", 3, 120),
            ("hodnoceni", 1, 90),
            ("rok", 1, 70),
        ]

        columns = tuple(col for col, _, _ in self.column_layout)
        self.tree = ttk.Treeview(self, columns=columns, show="headings", height=18)
        headings = {
            "nazev": ("Název", 240),
            "typ": ("Typ", 100),
            "zanr": ("Žánr", 120),
            "stav": ("Stav", 140),
            "hodnoceni": ("Hodnocení", 90),
            "rok": ("Rok", 70),
        }
        for col, (title, width) in headings.items():
            self.tree.heading(col, text=title)
            self.tree.column(col, width=width, anchor="w", stretch=True)
        self.tree.grid(row=0, column=0, sticky="nsew")

        y_scroll = ttk.Scrollbar(self, orient="vertical", command=self.tree.yview)
        y_scroll.grid(row=0, column=1, sticky="ns")
        x_scroll = ttk.Scrollbar(self, orient="horizontal", command=self.tree.xview)
        x_scroll.grid(row=1, column=0, sticky="ew")
        self.tree.configure(yscrollcommand=y_scroll.set, xscrollcommand=x_scroll.set)

        self.tree.bind("<Configure>", self._resize_columns)
        self._resize_columns()

    def insert_item(self, values):
        return self.tree.insert("", "end", values=values)

    def bind_select(self, callback):
        self.tree.bind("<<TreeviewSelect>>", callback)

    def selected_item(self):
        selected = self.tree.selection()
        return selected[0] if selected else None

    def item_values(self, item_id):
        return self.tree.item(item_id, "values")

    def clear(self):
        for item_id in self.tree.get_children():
            self.tree.delete(item_id)

    def select_item(self, item_id):
        self.tree.selection_set(item_id)
        self.tree.focus(item_id)
        self.tree.see(item_id)

    def _resize_columns(self, event=None):
        total_width = event.width if event else self.tree.winfo_width()
        if total_width <= 1:
            return
        min_total = sum(min_width for _, _, min_width in self.column_layout)
        weight_total = sum(weight for _, weight, _ in self.column_layout)
        extra = max(0, total_width - min_total)
        for col, weight, min_width in self.column_layout:
            width = min_width + int(extra * weight / weight_total)
            self.tree.column(col, width=width)
