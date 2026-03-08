import tkinter as tk
from tkinter import ttk
from menu import MenuBar
from treeview_panel import TreeviewPanel
from detail_notebook import DetailNotebook

TYPE_OPTIONS = [("Vše", "all"), ("film", "film"), ("anime", "anime"), ("seriál", "seriál")]
SAMPLE_DATA = [
    (("Inception", "film", "sci-fi", "shlédnuto", "9/10", "2010"), "Mind-bending heist in dreams."),
    (("Attack on Titan", "anime", "akční", "sleduji", "10/10", "2013"), "Dark fantasy with intense world building."),
    (("Breaking Bad", "seriál", "drama", "shlédnuto", "10/10", "2008"), "Chemistry teacher turns to crime."),
]
MENU_ACTION_HANDLERS = {
    "new": "_menu_new",
    "open": "_menu_open",
    "save": "_menu_save",
    "export": "_menu_export",
    "edit": "_menu_edit",
    "delete": "_menu_delete",
    "cut": "_menu_cut",
    "copy": "_menu_copy",
    "paste": "_menu_paste",
    "view_list": "_menu_view_list",
    "view_detail": "_menu_view_detail",
    "track_add": "_menu_track_add",
    "mark_watched": "_menu_mark_watched",
    "stats_overview": "_menu_stats_overview",
    "stats_by_genre": "_menu_stats_by_genre",
    "about": "_menu_about",
}

class MyApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Media Database")
        self.master.geometry("1400x900")

        self.current_data_id = None
        self.all_items = []
        self.items_by_id = {}
        self.visible_item_map = {}

        self._attach_menu()
        self._build_layout()
        self._load_sample_data()
        self._apply_filters()

    def _attach_menu(self):
        self.menu_bar = MenuBar(self.master, self._on_menu_action)
        self.master.config(menu=self.menu_bar.menu)

    def _build_layout(self):
        container = ttk.Frame(self.master, padding=12)
        container.pack(fill="both", expand=True)
        container.columnconfigure(0, weight=3)
        container.columnconfigure(1, weight=2)
        container.rowconfigure(1, weight=1)

        controls = ttk.Frame(container)
        controls.grid(row=0, column=0, columnspan=2, sticky="ew", pady=(0, 10))

        ttk.Label(controls, text="Hledat:").pack(side="left")
        self.search_var = tk.StringVar()
        search_entry = ttk.Entry(controls, textvariable=self.search_var, width=30)
        search_entry.pack(side="left", padx=(8, 16))
        search_entry.bind("<KeyRelease>", self._on_filter_change)

        ttk.Label(controls, text="Typ:").pack(side="left")
        self.type_var = tk.StringVar(value="all")
        type_frame = ttk.Frame(controls)
        type_frame.pack(side="left", padx=(8, 0))
        for label, value in TYPE_OPTIONS:
            ttk.Radiobutton(
                type_frame,
                text=label,
                value=value,
                variable=self.type_var,
                command=self._apply_filters,
            ).pack(side="left", padx=(0, 6))

        self.tree_panel = TreeviewPanel(container)
        self.tree_panel.grid(row=1, column=0, sticky="nsew", padx=(0, 8))
        self.tree_panel.bind_select(self._on_tree_select)

        self.detail_panel = DetailNotebook(container)
        self.detail_panel.grid(row=1, column=1, sticky="nsew")
        self.detail_panel.bind_note_change(self._on_notes_change)

    def _load_sample_data(self):
        self.all_items = []
        self.items_by_id = {}
        index = 1
        for values, note in SAMPLE_DATA:
            item = {"id": str(index), "values": values, "note": note}
            self.all_items.append(item)
            self.items_by_id[item["id"]] = item
            index += 1

    def _on_menu_action(self, action_id):
        handler_name = MENU_ACTION_HANDLERS.get(action_id)
        if handler_name is None:
            print(f"Unhandled menu action: {action_id}")
            return
        handler = getattr(self, handler_name)
        handler()

    def _menu_new(self):
        print("Menu action: new")

    def _menu_open(self):
        print("Menu action: open")

    def _menu_save(self):
        print("Menu action: save")

    def _menu_export(self):
        print("Menu action: export")

    def _menu_edit(self):
        print("Menu action: edit")

    def _menu_delete(self):
        print("Menu action: delete")

    def _menu_cut(self):
        print("Menu action: cut")

    def _menu_copy(self):
        print("Menu action: copy")

    def _menu_paste(self):
        print("Menu action: paste")

    def _menu_view_list(self):
        print("Menu action: view_list")

    def _menu_view_detail(self):
        print("Menu action: view_detail")

    def _menu_track_add(self):
        print("Menu action: track_add")

    def _menu_mark_watched(self):
        print("Menu action: mark_watched")

    def _menu_stats_overview(self):
        print("Menu action: stats_overview")

    def _menu_stats_by_genre(self):
        print("Menu action: stats_by_genre")

    def _menu_about(self):
        print("Menu action: about")

    def _on_notes_change(self, _event):
        self._save_current_note()

    def _on_filter_change(self, _event):
        self._apply_filters()

    def _on_tree_select(self, _event):
        self._save_current_note()
        tree_item_id = self.tree_panel.selected_item()
        selected_item = self.visible_item_map.get(tree_item_id)
        if selected_item:
            self._show_item(selected_item)

    def _save_current_note(self):
        if not self.current_data_id:
            return
        item = self.items_by_id.get(self.current_data_id)
        if item:
            item["note"] = self.detail_panel.get_note()

    def _show_item(self, item):
        self.current_data_id = item["id"]
        self.detail_panel.set_details(item["values"])
        self.detail_panel.set_note(item["note"])

    def _matches_filters(self, item, search_text, selected_type):
        values = item["values"]
        row_text = " ".join(values).lower()
        type_match = selected_type == "all" or values[1].lower() == selected_type
        text_match = not search_text or search_text in row_text
        return type_match and text_match

    def _apply_filters(self):
        self._save_current_note()
        search_text = self.search_var.get().strip().lower()
        selected_type = self.type_var.get().strip().lower()

        filtered_items = []
        for item in self.all_items:
            if self._matches_filters(item, search_text, selected_type):
                filtered_items.append(item)

        previous_data_id = self.current_data_id
        self.tree_panel.clear()
        self.visible_item_map = {}

        selected_tree_id = None
        for item in filtered_items:
            tree_item_id = self.tree_panel.insert_item(item["values"])
            self.visible_item_map[tree_item_id] = item
            if item["id"] == previous_data_id:
                selected_tree_id = tree_item_id

        if not selected_tree_id and filtered_items:
            selected_tree_id = next(iter(self.visible_item_map))

        if selected_tree_id:
            self.tree_panel.select_item(selected_tree_id)
            self._show_item(self.visible_item_map[selected_tree_id])
        else:
            self.current_data_id = None
            self.detail_panel.clear_details()
            self.detail_panel.set_note("")


if __name__ == "__main__":
    root = tk.Tk()
    app = MyApp(root)
    root.mainloop()
