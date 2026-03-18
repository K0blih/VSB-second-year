import tkinter as tk
import json
from tkinter import filedialog, messagebox, ttk
from menu import MenuBar
from treeview_panel import TreeviewPanel
from detail_notebook import DetailNotebook, TYPE_CHOICES
from media_library import DEFAULT_DETAILS, MediaLibrary

TYPE_FILTER_OPTIONS = [("Vše", "all"), *[(value, value) for value in TYPE_CHOICES]]

class MyApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Media Database")
        self.master.geometry("1400x900")

        self.current_data_id = None
        self.library = MediaLibrary()
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
        for label, value in TYPE_FILTER_OPTIONS:
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
        self.detail_panel.bind_detail_change(self._on_details_change)
        self.detail_panel.bind_note_change(self._on_notes_change)

    def _load_sample_data(self):
        self.library.load_sample_data()

    def _on_menu_action(self, action_id):
        handler = getattr(self, f"_menu_{action_id}", None)
        if handler is None:
            print(f"Unhandled menu action: {action_id}")
            return
        handler()

    def _menu_new(self):
        item = self.library.append_item(DEFAULT_DETAILS.copy(), "")
        self._apply_filters(select_data_id=item["id"])
        self.detail_panel.focus_name()

    def _menu_open(self):
        file_path = filedialog.askopenfilename(
            parent=self.master,
            title="Otevřít databázi",
            filetypes=[("JSON soubory", "*.json"), ("Všechny soubory", "*.*")],
        )
        if not file_path:
            return
        try:
            with open(file_path, "r", encoding="utf-8") as file_handle:
                payload = json.load(file_handle)
        except (OSError, json.JSONDecodeError) as exc:
            messagebox.showerror("Otevřít", f"Soubor se nepodařilo načíst.\n{exc}", parent=self.master)
            return

        items = payload.get("items")
        if not isinstance(items, list):
            messagebox.showerror("Otevřít", "Soubor nemá platný formát databáze.", parent=self.master)
            return

        self._load_items(items)
        self._apply_filters()
        messagebox.showinfo("Otevřít", f"Načteno {len(self.library.all_items)} položek.", parent=self.master)

    def _menu_save(self):
        self._save_current_note()
        file_path = filedialog.asksaveasfilename(
            parent=self.master,
            title="Uložit databázi",
            defaultextension=".json",
            filetypes=[("JSON soubory", "*.json"), ("Všechny soubory", "*.*")],
        )
        if not file_path:
            return
        payload = {"items": self.library.serialize_items()}
        try:
            with open(file_path, "w", encoding="utf-8") as file_handle:
                json.dump(payload, file_handle, ensure_ascii=False, indent=2)
        except OSError as exc:
            messagebox.showerror("Uložit", f"Soubor se nepodařilo uložit.\n{exc}", parent=self.master)
            return
        messagebox.showinfo("Uložit", "Databáze byla uložena.", parent=self.master)

    def _menu_export(self):
        self._save_current_note()
        file_path = filedialog.asksaveasfilename(
            parent=self.master,
            title="Exportovat seznam",
            defaultextension=".csv",
            filetypes=[("CSV soubory", "*.csv"), ("Všechny soubory", "*.*")],
        )
        if not file_path:
            return
        try:
            with open(file_path, "w", encoding="utf-8", newline="") as file_handle:
                file_handle.writelines(self.library.export_rows())
        except OSError as exc:
            messagebox.showerror("Export", f"Export se nepodařil.\n{exc}", parent=self.master)
            return
        messagebox.showinfo("Export", "Export do CSV byl dokončen.", parent=self.master)

    def _menu_delete(self):
        item = self._current_item()
        if not item:
            messagebox.showinfo("Smazat", "Nejprve vyber položku v seznamu.", parent=self.master)
            return
        item_name = item["details"]["name"] or "Bez názvu"
        confirmed = messagebox.askyesno(
            "Smazat položku",
            f"Opravdu chceš smazat položku „{item_name}“?",
            parent=self.master,
        )
        if not confirmed:
            return
        self.library.delete_item(item["id"])
        self.current_data_id = None
        self._apply_filters()

    def _menu_view_list(self):
        self.tree_panel.focus_tree()

    def _menu_view_detail(self):
        if not self.current_data_id:
            messagebox.showinfo("Detail položky", "Nejprve vyber položku v seznamu.", parent=self.master)
            return
        self.detail_panel.show_detail_tab()
        self.detail_panel.focus_name()

    def _menu_stats_overview(self):
        self._show_modal("Statistiky - přehled", "\n".join(self.library.overview_lines()))

    def _menu_stats_by_genre(self):
        self._show_modal("Statistiky - žánry", "\n".join(self.library.genre_lines()))

    def _menu_about(self):
        body = (
            "Media Database\n\n"
            "Desktopová aplikace pro evidenci filmů, anime a seriálů.\n"
            "Menu je napojené na správu položek, filtrování, statistiky a práci se soubory.\n\n"
            "Autor: Richard Chovanec\n"
            "Login: CHO0289"
        )
        self._show_modal("O aplikaci", body)

    def _on_notes_change(self, _event):
        self._save_current_note()
        self._refresh_selected_row()

    def _on_details_change(self):
        if not self.current_data_id:
            return
        item = self.library.get_item(self.current_data_id)
        if not item:
            return
        item = self.library.update_item_details(self.current_data_id, self.detail_panel.get_details())
        search_text = self.search_var.get().strip().lower()
        selected_type = self.type_var.get().strip().lower()
        if self.library.matches_filters(item, search_text, selected_type):
            self._refresh_selected_row()
            return
        self._apply_filters()

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
        self.library.update_item_note(self.current_data_id, self.detail_panel.get_note())

    def _current_item(self):
        if not self.current_data_id:
            return None
        return self.library.get_item(self.current_data_id)

    def _load_items(self, serialized_items):
        self.current_data_id = None
        self.library.load_items(serialized_items)

    def _show_modal(self, title, body):
        dialog = tk.Toplevel(self.master)
        dialog.title(title)
        dialog.transient(self.master)
        dialog.grab_set()
        dialog.resizable(True, True)
        dialog.minsize(520, 320)

        frame = ttk.Frame(dialog, padding=16)
        frame.pack(fill="both", expand=True)
        message = tk.Text(frame, wrap="word", height=12, width=60)
        message.insert("1.0", body)
        message.configure(state="disabled")
        message.pack(fill="both", expand=True)
        ttk.Button(frame, text="Zavřít", command=dialog.destroy).pack(anchor="e", pady=(12, 0))

        dialog.update_idletasks()
        width = max(520, dialog.winfo_reqwidth())
        height = max(320, dialog.winfo_reqheight())
        x = self.master.winfo_rootx() + max(20, (self.master.winfo_width() - width) // 2)
        y = self.master.winfo_rooty() + max(20, (self.master.winfo_height() - height) // 2)
        dialog.geometry(f"{width}x{height}+{x}+{y}")
        dialog.wait_window()

    def _refresh_selected_row(self):
        tree_item_id = self.tree_panel.selected_item()
        if not tree_item_id or tree_item_id not in self.visible_item_map:
            return
        item = self.visible_item_map[tree_item_id]
        self.tree_panel.update_item(tree_item_id, self.library.tree_values_for_item(item))

    def _show_item(self, item):
        self.current_data_id = item["id"]
        self.detail_panel.set_details(item["details"])
        self.detail_panel.set_note(item["note"])

    def _apply_filters(self, select_data_id=None):
        self._save_current_note()
        search_text = self.search_var.get().strip().lower()
        selected_type = self.type_var.get().strip().lower()
        filtered_items = self.library.filter_items(search_text, selected_type)

        previous_data_id = select_data_id if select_data_id is not None else self.current_data_id
        self.tree_panel.clear()
        self.visible_item_map = {}

        selected_tree_id = None
        for item in filtered_items:
            tree_item_id = self.tree_panel.insert_item(self.library.tree_values_for_item(item))
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
