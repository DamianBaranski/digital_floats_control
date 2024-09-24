try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class TableWidget(tk.Frame):
    def __init__(self, parent, columns):
        # Define column headers, widths (as percentages of total width), and data types
        self.columns = columns
        tk.Frame.__init__(self, parent)
        self.entries = []  # Store references to Entry widgets for navigation

        # Configure column weights for the table based on defined widths
        for i, column in enumerate(self.columns):
            self.grid_columnconfigure(i, weight=1, minsize=50)

        # Create the header row
        for i, column in enumerate(self.columns):
            a = tk.Label(self, text=column['name'], wraplength=50)
            a.config(font=("Courier", 6))
            a.grid(row=0, column=i, sticky="nsew")

        # Create the table rows with appropriate widgets (Entry or Checkbutton)
        for i in range(6):
            row_entries = []
            for j, column in enumerate(self.columns):
                if column["type"] == "bool":
                    # Create a checkbox for boolean values
                    var = tk.BooleanVar()
                    b = tk.Frame(self, background='white', highlightthickness=1, highlightbackground='red', highlightcolor='red')
                    a = tk.Checkbutton(b, variable=var)
                    a.pack(fill='none', expand=True)

                    row_entries.append(var)  # Store BooleanVar for the checkbox
                else:
                    # Create Entry widget for other types
                    b = Entry(self, validate="key")
                    
                    # Bind data validation functions according to column type
                    if column["type"] == "int":
                        b.config(validatecommand=(self.register(self.validate_int), '%P'))  # Validate integer input
                    elif column["type"] == "str":
                        b.config(validatecommand=(self.register(self.validate_str), '%P'))  # Validate string input

                    row_entries.append(b)  # Store Entry widget

                b.grid(row=i+1, column=j, sticky="nsew")
                #b['state'] = "readonly"

                if column["type"] != "bool":
                    # Bind Enter, Up, Down keys for navigation, only for Entry widgets
                    b.bind("<Return>", self.focus_next_cell)
                    b.bind("<Down>", self.focus_next_row)
                    b.bind("<Up>", self.focus_prev_row)
                    
            self.entries.append(row_entries)

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
