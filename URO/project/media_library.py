from collections import Counter

from detail_notebook import STATUS_CHOICES, TYPE_CHOICES


DEFAULT_DETAILS = {
    "name": "",
    "type": "film",
    "genre": "",
    "status": "plánuji",
    "rating": None,
    "year": None,
}

SAMPLE_DATA = [
    ({"name": "Inception", "type": "film", "genre": "sci-fi", "status": "shlédnuto", "rating": 9, "year": 2010}, "Mind-bending heist in dreams."),
    ({"name": "Attack on Titan", "type": "anime", "genre": "akční", "status": "sleduji", "rating": 10, "year": 2013}, "Dark fantasy with intense world building."),
    ({"name": "Breaking Bad", "type": "seriál", "genre": "drama", "status": "shlédnuto", "rating": 10, "year": 2008}, "Chemistry teacher turns to crime."),
]


class MediaLibrary:
    def __init__(self):
        self.reset()

    def reset(self):
        self.next_item_id = 1
        self.all_items = []
        self.items_by_id = {}

    def load_sample_data(self):
        self.reset()
        for details, note in SAMPLE_DATA:
            self.append_item(details.copy(), note)

    def append_item(self, details, note):
        item = {
            "id": str(self.next_item_id),
            "details": self.normalize_details(details),
            "note": str(note),
        }
        self.next_item_id += 1
        self.all_items.append(item)
        self.items_by_id[item["id"]] = item
        return item

    def get_item(self, item_id):
        return self.items_by_id.get(item_id)

    def update_item_details(self, item_id, details):
        item = self.get_item(item_id)
        if item:
            item["details"] = self.normalize_details(details)
        return item

    def update_item_note(self, item_id, note):
        item = self.get_item(item_id)
        if item:
            item["note"] = str(note)
        return item

    def delete_item(self, item_id):
        self.all_items = [item for item in self.all_items if item["id"] != item_id]
        self.items_by_id.pop(item_id, None)

    def load_items(self, serialized_items):
        self.reset()
        for entry in serialized_items:
            if not isinstance(entry, dict):
                continue
            details = entry.get("details", {})
            note = entry.get("note", "")
            self.append_item(details if isinstance(details, dict) else {}, note)

    def serialize_items(self):
        return [{"details": item["details"], "note": item["note"]} for item in self.all_items]

    def export_rows(self):
        rows = ["Název;Typ;Žánr;Stav;Hodnocení;Rok;Poznámka\n"]
        for item in self.all_items:
            details = item["details"]
            row = [
                details["name"],
                details["type"],
                details["genre"],
                details["status"],
                self.format_rating(details["rating"]),
                self.format_year(details["year"]),
                item["note"].replace("\n", " ").strip(),
            ]
            escaped = [value.replace(";", ",") for value in row]
            rows.append(";".join(escaped) + "\n")
        return rows

    def tree_values_for_item(self, item):
        details = item["details"]
        return (
            details["name"],
            details["type"],
            details["genre"],
            details["status"],
            self.format_rating(details["rating"]),
            self.format_year(details["year"]),
            self.note_preview(item["note"]),
        )

    def filter_items(self, search_text, selected_type):
        return [item for item in self.all_items if self.matches_filters(item, search_text, selected_type)]

    def matches_filters(self, item, search_text, selected_type):
        details = item["details"]
        row_text = " ".join(
            (
                details["name"],
                details["type"],
                details["genre"],
                details["status"],
                self.format_rating(details["rating"]),
                self.format_year(details["year"]),
            )
        ).lower()
        notes_text = item["note"].lower()
        type_match = selected_type == "all" or details["type"].lower() == selected_type
        text_match = not search_text or search_text in row_text or search_text in notes_text
        return type_match and text_match

    def overview_lines(self):
        total_items = len(self.all_items)
        watched_items = sum(1 for item in self.all_items if item["details"]["status"] == "shlédnuto")
        in_progress_items = sum(1 for item in self.all_items if item["details"]["status"] == "sleduji")
        rated_items = [item["details"]["rating"] for item in self.all_items if item["details"]["rating"] is not None]
        return [
            f"Počet položek: {total_items}",
            f"Shlédnuto: {watched_items}",
            f"Právě sleduji: {in_progress_items}",
            f"Průměrné hodnocení: {sum(rated_items) / len(rated_items):.1f}/10" if rated_items else "Průměrné hodnocení: bez dat",
        ]

    def genre_lines(self):
        counts = Counter(item["details"]["genre"] or "neuvedeno" for item in self.all_items)
        if not counts:
            return ["Databáze je prázdná."]
        ordered = sorted(counts.items(), key=lambda pair: (-pair[1], pair[0]))
        return [f"{genre}: {count}" for genre, count in ordered]

    def normalize_details(self, details):
        normalized = DEFAULT_DETAILS.copy()
        normalized.update(details)
        rating = normalized.get("rating")
        year = normalized.get("year")
        normalized["rating"] = rating if isinstance(rating, int) and 1 <= rating <= 10 else None
        normalized["year"] = year if isinstance(year, int) and 1900 <= year <= 2100 else None
        normalized["name"] = str(normalized.get("name", "")).strip()
        normalized_type = str(normalized.get("type", DEFAULT_DETAILS["type"])).strip()
        normalized["type"] = normalized_type if normalized_type in TYPE_CHOICES else DEFAULT_DETAILS["type"]
        normalized["genre"] = str(normalized.get("genre", "")).strip()
        normalized_status = str(normalized.get("status", DEFAULT_DETAILS["status"])).strip()
        normalized["status"] = normalized_status if normalized_status in STATUS_CHOICES else DEFAULT_DETAILS["status"]
        return normalized

    def note_preview(self, text):
        normalized = " ".join(text.split())
        if len(normalized) <= 24:
            return normalized
        return f"{normalized[:21]}..."

    def format_rating(self, rating):
        return "" if rating is None else f"{rating}/10"

    def format_year(self, year):
        return "" if year is None else str(year)
