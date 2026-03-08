import tkinter as tk
from tkinter import ttk


class DetailNotebook(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)

        self.notebook = ttk.Notebook(self)
        self.notebook.grid(row=0, column=0, sticky="nsew")

        detail_tab = ttk.Frame(self.notebook, padding=10)
        notes_tab = ttk.Frame(self.notebook, padding=10)
        self.notebook.add(detail_tab, text="Detail")
        self.notebook.add(notes_tab, text="Poznámky")

        detail_tab.columnconfigure(1, weight=1)
        self.detail_keys = ["Název", "Typ", "Žánr", "Stav", "Hodnocení", "Rok"]
        self.detail_vars = {key: tk.StringVar() for key in self.detail_keys}

        for row, key in enumerate(self.detail_keys):
            ttk.Label(detail_tab, text=f"{key}:").grid(row=row, column=0, sticky="w", pady=4, padx=(0, 10))
            ttk.Entry(detail_tab, textvariable=self.detail_vars[key]).grid(row=row, column=1, sticky="ew", pady=4)

        self.notes_text = tk.Text(notes_tab, height=16, wrap="word")
        self.notes_text.pack(fill="both", expand=True)

    def set_details(self, values):
        for i, key in enumerate(self.detail_keys):
            self.detail_vars[key].set(values[i] if i < len(values) else "")

    def clear_details(self):
        for key in self.detail_keys:
            self.detail_vars[key].set("")

    def set_note(self, text):
        self.notes_text.delete("1.0", "end")
        self.notes_text.insert("1.0", text)

    def get_note(self):
        return self.notes_text.get("1.0", "end-1c")

    def bind_note_change(self, callback):
        self.notes_text.bind("<KeyRelease>", callback)
