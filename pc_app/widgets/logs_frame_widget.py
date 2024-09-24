try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class LogsFrameWidget(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent)
        
        # Configure the grid layout for better control
        self.grid_rowconfigure(1, weight=1)  # Text widget row should expand
        self.grid_columnconfigure(0, weight=1)  # Make sure it expands horizontally
        
        # Label at the top
        self.label = tk.Label(self, text="Logs:")
        self.label.grid(row=0, column=0, sticky="ew")  # Stick to top, expand horizontally
        
        # Text widget that expands in both directions with horizontal padding (padx)
        self.log = tk.Text(self)
        self.log.grid(row=1, column=0, sticky="nsew", padx=10)  # Add horizontal padding with padx=10
        
        # Clear button positioned under the Text widget
        self.clear_button = tk.Button(self, text="Clear")
        self.clear_button.grid(row=2, column=0)