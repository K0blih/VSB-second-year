import tkinter as tk
from tkinter import ttk


TYPE_CHOICES = ("film", "anime", "seriál")
GENRE_CHOICES = ("akční", "drama", "fantasy", "komedie", "sci-fi", "thriller")
STATUS_CHOICES = ("plánuji", "sleduji", "shlédnuto", "odloženo")
RATING_CHOICES = tuple(f"{value}/10" for value in range(1, 11))


class DetailNotebook(ttk.Frame):
    def __init__(self, parent):
        super().__init__(parent)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(0, weight=1)
        self._suspend_callbacks = False
        self._detail_change_callback = None

        self.name_var = tk.StringVar()
        self.type_var = tk.StringVar(value=TYPE_CHOICES[0])
        self.genre_var = tk.StringVar()
        self.status_var = tk.StringVar(value=STATUS_CHOICES[0])
        self.rating_var = tk.StringVar()
        self.year_var = tk.StringVar()

        self.notebook = ttk.Notebook(self)
        self.notebook.grid(row=0, column=0, sticky="nsew")

        detail_tab = ttk.Frame(self.notebook, padding=10)
        notes_tab = ttk.Frame(self.notebook, padding=10)
        self.notebook.add(detail_tab, text="Detail")
        self.notebook.add(notes_tab, text="Poznámky")

        detail_tab.columnconfigure(1, weight=1)
        detail_tab.columnconfigure(2, weight=1)

        ttk.Label(detail_tab, text="Název:").grid(row=0, column=0, sticky="w", pady=4, padx=(0, 10))
        self.name_entry = ttk.Entry(detail_tab, textvariable=self.name_var)
        self.name_entry.grid(row=0, column=1, columnspan=2, sticky="ew", pady=4)

        ttk.Label(detail_tab, text="Typ:").grid(row=1, column=0, sticky="w", pady=4, padx=(0, 10))
        ttk.Combobox(
            detail_tab,
            textvariable=self.type_var,
            values=TYPE_CHOICES,
            state="readonly",
        ).grid(row=1, column=1, columnspan=2, sticky="ew", pady=4)

        ttk.Label(detail_tab, text="Žánr:").grid(row=2, column=0, sticky="w", pady=4, padx=(0, 10))
        ttk.Combobox(
            detail_tab,
            textvariable=self.genre_var,
            values=GENRE_CHOICES,
        ).grid(row=2, column=1, columnspan=2, sticky="ew", pady=4)

        ttk.Label(detail_tab, text="Stav:").grid(row=3, column=0, sticky="nw", pady=4, padx=(0, 10))
        status_frame = ttk.Frame(detail_tab)
        status_frame.grid(row=3, column=1, columnspan=2, sticky="w", pady=4)
        for index, status in enumerate(STATUS_CHOICES):
            ttk.Radiobutton(
                status_frame,
                text=status,
                value=status,
                variable=self.status_var,
            ).grid(row=index // 2, column=index % 2, sticky="w", padx=(0, 12), pady=2)

        ttk.Label(detail_tab, text="Hodnocení:").grid(row=4, column=0, sticky="w", pady=4, padx=(0, 10))
        rating_values = ("",) + RATING_CHOICES
        ttk.Combobox(
            detail_tab,
            textvariable=self.rating_var,
            values=rating_values,
            state="readonly",
        ).grid(row=4, column=1, sticky="ew", pady=4)

        year_frame = ttk.Frame(detail_tab)
        year_frame.grid(row=4, column=2, sticky="e", pady=4)
        ttk.Label(year_frame, text="Rok:").pack(side="left", padx=(0, 8))
        ttk.Spinbox(
            year_frame,
            from_=1900,
            to=2100,
            textvariable=self.year_var,
            width=8,
        ).pack(side="left")

        self.notes_text = tk.Text(notes_tab, height=16, wrap="word")
        self.notes_text.pack(fill="both", expand=True)

        for variable in (
            self.name_var,
            self.type_var,
            self.genre_var,
            self.status_var,
            self.rating_var,
            self.year_var,
        ):
            variable.trace_add("write", self._handle_detail_change)

    def set_details(self, details):
        self._suspend_callbacks = True
        self.name_var.set(details.get("name", ""))
        self.type_var.set(self._normalize_choice(details.get("type"), TYPE_CHOICES, TYPE_CHOICES[0]))
        self.genre_var.set(details.get("genre", ""))
        self.status_var.set(self._normalize_choice(details.get("status"), STATUS_CHOICES, STATUS_CHOICES[0]))

        rating = details.get("rating")
        self.rating_var.set("" if rating is None else f"{rating}/10")

        year = details.get("year")
        self.year_var.set("" if year is None else str(year))
        self._suspend_callbacks = False

    def clear_details(self):
        self.set_details({})

    def set_note(self, text):
        self._suspend_callbacks = True
        self.notes_text.delete("1.0", "end")
        self.notes_text.insert("1.0", text)
        self._suspend_callbacks = False

    def get_note(self):
        return self.notes_text.get("1.0", "end-1c")

    def get_details(self):
        return {
            "name": self.name_var.get().strip(),
            "type": self._normalize_choice(self.type_var.get(), TYPE_CHOICES, TYPE_CHOICES[0]),
            "genre": self.genre_var.get().strip(),
            "status": self._normalize_choice(self.status_var.get(), STATUS_CHOICES, STATUS_CHOICES[0]),
            "rating": self._parse_rating(self.rating_var.get()),
            "year": self._parse_year(self.year_var.get()),
        }

    def bind_detail_change(self, callback):
        self._detail_change_callback = callback

    def bind_note_change(self, callback):
        self.notes_text.bind("<KeyRelease>", callback)

    def show_detail_tab(self):
        self.notebook.select(0)

    def focus_name(self):
        self.show_detail_tab()
        self.name_entry.focus_set()

    def _handle_detail_change(self, *_args):
        if self._suspend_callbacks or self._detail_change_callback is None:
            return
        self._detail_change_callback()

    def _normalize_choice(self, value, allowed_values, default):
        return value if value in allowed_values else default

    def _parse_rating(self, value):
        if not value:
            return None
        try:
            numeric_value = int(value.split("/", 1)[0])
        except (TypeError, ValueError):
            return None
        return numeric_value if 1 <= numeric_value <= 10 else None

    def _parse_year(self, value):
        value = value.strip()
        if not value:
            return None
        try:
            year = int(value)
        except ValueError:
            return None
        return year if 1900 <= year <= 2100 else None
