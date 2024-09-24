try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class TableWidget(tk.Frame):
    def __init__(self, parent, columns):
        self.columns = columns
        tk.Frame.__init__(self, parent)

        # Create a canvas to hold the table
        self.canvas = tk.Canvas(self)
        self.canvas.pack(side="top", fill="both", expand=True)

        # Create a horizontal scrollbar placed at the bottom
        self.h_scrollbar = tk.Scrollbar(self, orient="horizontal", command=self.canvas.xview)
        self.h_scrollbar.pack(side="bottom", fill="x")

        self.canvas.configure(xscrollcommand=self.h_scrollbar.set)

        # Create a frame inside the canvas for the table content
        self.table_frame = tk.Frame(self.canvas)
        self.canvas.create_window((0, 0), window=self.table_frame, anchor="nw")

        # Bind the canvas scroll region
        self.table_frame.bind("<Configure>", self.on_frame_configure)

        # Bind window resizing events to adjust scrollbar visibility
        self.canvas.bind("<Configure>", self.on_canvas_resize)

        self.entries = []  # Store references to Entry widgets for navigation

        # Configure column weights for the table based on defined widths
        for i, column in enumerate(self.columns):
            self.table_frame.grid_columnconfigure(i, weight=1, minsize=85)

        # Create the header row
        for i, column in enumerate(self.columns):
            a = tk.Label(self.table_frame, text=column['name'], wraplength=85)
            a.config(font=("Courier", 10))
            a.grid(row=0, column=i, sticky="nsew")

        # Create the table rows with appropriate widgets (Entry or Checkbutton)
        for i in range(6):
            row_entries = []
            for j, column in enumerate(self.columns):
                if column["type"] == "bool":
                    # Create a checkbox for boolean values
                    var = tk.BooleanVar()
                    color = self.cget("bg")
                    b = tk.Frame(self.table_frame, background='white', highlightthickness=1, highlightbackground=color, highlightcolor=color)
                    a = tk.Checkbutton(b, variable=var,  background='white')
                    a.pack(fill='none', expand=True)

                    row_entries.append(var)  # Store BooleanVar for the checkbox
                else:
                    # Create Entry widget for other types
                    b = Entry(self.table_frame, validate="key")
                    
                    # Bind data validation functions according to column type
                    if column["type"] == "int":
                        b.config(validatecommand=(self.register(self.validate_int), '%P'))  # Validate integer input
                    elif column["type"] == "str":
                        b.config(validatecommand=(self.register(self.validate_str), '%P'))  # Validate string input

                    row_entries.append(b)  # Store Entry widget

                b.grid(row=i+1, column=j, sticky="nsew")
                if column["type"] != "bool":
                    # Bind Enter, Up, Down keys for navigation, only for Entry widgets
                    b.bind("<Return>", self.focus_next_cell)
                    b.bind("<Down>", self.focus_next_row)
                    b.bind("<Up>", self.focus_prev_row)

            self.entries.append(row_entries)

    def on_frame_configure(self, event):
        """Reset the scroll region to encompass the inner frame."""
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))

    def on_canvas_resize(self, event):
        """Hide or show the scrollbar based on the table size relative to the canvas width."""
        canvas_width = event.width
        table_width = self.table_frame.winfo_reqwidth()

        # If table is smaller than canvas, hide the scrollbar
        if table_width <= canvas_width:
            self.canvas.itemconfig(self.canvas.create_window((0, 0), window=self.table_frame), width=canvas_width)
            self.h_scrollbar.pack_forget()
        else:
            self.h_scrollbar.pack(side="bottom", fill="x")

    def validate_int(self, value_if_allowed):
        """Ensure only integer input is allowed."""
        if value_if_allowed == "" or value_if_allowed.isdigit():
            return True
        return False

    def validate_str(self, value_if_allowed):
        """Ensure only string input (no restrictions for now) is allowed."""
        return True  # No restrictions for string input

    def focus_next_cell(self, event):
        """Focus on the next cell when Enter is pressed."""
        current_widget = event.widget
        current_index = None

        # Find current widget in the grid
        for i, row in enumerate(self.entries):
            if current_widget in row:
                current_index = (i, row.index(current_widget))
                break

        if current_index:
            next_row, next_col = current_index[0], current_index[1] + 1
            if next_col >= len(self.columns):
                next_row += 1
                next_col = 0

            # If the next cell exists, focus on it
            if next_row < len(self.entries):
                next_widget = self.entries[next_row][next_col]
                if isinstance(next_widget, tk.Entry):  # Only focus on Entry widgets
                    next_widget.focus()

    def focus_next_row(self, event):
        """Focus on the cell directly below the current one."""
        current_widget = event.widget
        current_index = None

        # Find current widget in the grid
        for i, row in enumerate(self.entries):
            if current_widget in row:
                current_index = (i, row.index(current_widget))
                break

        if current_index:
            next_row, col = current_index[0] + 1, current_index[1]
            if next_row < len(self.entries):
                next_widget = self.entries[next_row][col]
                if isinstance(next_widget, tk.Entry):  # Only focus on Entry widgets
                    next_widget.focus()

    def focus_prev_row(self, event):
        """Focus on the cell directly above the current one."""
        current_widget = event.widget
        current_index = None

        # Find current widget in the grid
        for i, row in enumerate(self.entries):
            if current_widget in row:
                current_index = (i, row.index(current_widget))
                break

        if current_index:
            prev_row, col = current_index[0] - 1, current_index[1]
            if prev_row >= 0:
                prev_widget = self.entries[prev_row][col]
                if isinstance(prev_widget, tk.Entry):  # Only focus on Entry widgets
                    prev_widget.focus()
