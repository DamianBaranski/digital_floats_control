import tkinter as tk
from tkinter import ttk, filedialog
import csv
import json
from datetime import datetime
from ....ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, SUCCESS, ERROR, FONT, HEADER_FONT

MOCK_DATA = [
    ["FL", "Deploy", "2025-05-07 12:00", "2025-05-07 12:00", 1.5, "Success"],
    ["FR", "Retract", "2025-05-07 12:01", "2025-05-07 12:01", 1.6, "Success"],
    ["RR", "Deploy", "2025-05-07 12:02", "2025-05-07 12:02", 1.4, "Timeout"],
    ["RUDDER", "Test Left", "2025-05-07 12:03", "2025-05-07 12:03", 0.9, "Success"],
    ["FL", "Retract", "2025-05-07 12:04", "2025-05-07 12:04", 1.7, "Failure"],
    ["FR", "Deploy", "2025-05-07 12:05", "2025-05-07 12:05", 1.2, "Warning"],
]

COLUMNS = ["Component", "Action", "Start Time", "End Time", "Duration (s)", "Status"]

class LogTablePanel(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent, bg=DARK_BG)
        self.data = MOCK_DATA.copy()
        self.filtered_data = self.data.copy()
        self.create_widgets()
        self.populate_table(self.filtered_data)

    def create_widgets(self):
        # Search/filter bar
        filter_frame = tk.Frame(self, bg=DARK_BG)
        filter_frame.pack(fill="x", padx=5, pady=2)
        tk.Label(filter_frame, text="Search:", bg=DARK_BG, fg=TEXT_COLOR, font=FONT).pack(side="left")
        self.search_var = tk.StringVar()
        self.search_var.trace_add("write", self.on_search)
        search_entry = tk.Entry(filter_frame, textvariable=self.search_var, 
                              bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                              insertbackground=TEXT_COLOR)
        search_entry.pack(side="left", padx=5)
        
        export_frame = tk.Frame(filter_frame, bg=DARK_BG)
        export_frame.pack(side="right")
        
        tk.Button(export_frame, text="Export CSV", command=self.export_csv,
                 bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                 activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR).pack(side="right", padx=2)
        tk.Button(export_frame, text="Export JSON", command=self.export_json,
                 bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                 activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR).pack(side="right", padx=2)

        # Table
        style = ttk.Style()
        style.configure("Treeview", background=DARKER_BG, foreground=TEXT_COLOR, fieldbackground=DARKER_BG)
        style.configure("Treeview.Heading", background=DARK_BG, foreground=TEXT_COLOR, font=FONT)
        style.map('Treeview', background=[('selected', BORDER_COLOR)])
        
        self.tree = ttk.Treeview(self, columns=COLUMNS, show="headings", height=12)
        for col in COLUMNS:
            self.tree.heading(col, text=col, command=lambda c=col: self.sort_by(c, False))
            self.tree.column(col, anchor="center", width=110)
        self.tree.pack(fill="both", expand=True, padx=5, pady=2)

        # Scrollbar
        scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        scrollbar.pack(side="right", fill="y")

    def populate_table(self, data):
        for row in self.tree.get_children():
            self.tree.delete(row)
        for row in data[-50:]:  # Show last 50
            tag = self.get_row_tag(row)
            # No wrapping, just display as is
            self.tree.insert("", "end", values=row, tags=(tag,))
        # Muted, dark-theme-friendly colors
        self.tree.tag_configure("Success", background="#26734d")   # dark green
        self.tree.tag_configure("Failure", background="#a93226")   # dark red
        self.tree.tag_configure("Timeout", background="#b9770e")   # dark orange
        self.tree.tag_configure("Warning", background="#b9770e")   # dark orange

    def get_row_tag(self, row):
        status = row[-1]
        if status == "Success":
            return "Success"
        elif status == "Failure":
            return "Failure"
        elif status == "Timeout" or status == "Warning":
            return "Warning"
        return ""

    def on_search(self, *args):
        query = self.search_var.get().lower()
        self.filtered_data = [row for row in self.data if query in str(row).lower()]
        self.populate_table(self.filtered_data)

    def sort_by(self, col, descending):
        col_idx = COLUMNS.index(col)
        def sort_key(row):
            if col in ["Duration (s)"]:
                return float(row[col_idx])
            if col in ["Start Time", "End Time"]:
                try:
                    return datetime.strptime(row[col_idx], "%Y-%m-%d %H:%M")
                except:
                    return row[col_idx]
            return row[col_idx]
        self.filtered_data.sort(key=sort_key, reverse=descending)
        self.populate_table(self.filtered_data)
        # Reverse sort next time
        self.tree.heading(col, text=col, command=lambda: self.sort_by(col, not descending))

    def export_csv(self):
        file = filedialog.asksaveasfilename(defaultextension=".csv", filetypes=[("CSV files", "*.csv")])
        if file:
            with open(file, "w", newline="") as f:
                writer = csv.writer(f)
                writer.writerow(COLUMNS)
                writer.writerows(self.filtered_data)

    def export_json(self):
        file = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if file:
            with open(file, "w") as f:
                json.dump([dict(zip(COLUMNS, row)) for row in self.filtered_data], f, indent=2) 