try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
    import tkinter.scrolledtext as scrolledtext
except ImportError:
    # python 2.x
    import Tkinter as tk
    import Tkinter.scrolledtext as scrolledtext

class LogsFrameWidget(tk.Frame):
    def __init__(self, parent, app_protocol):
        tk.Frame.__init__(self, parent)
        self.app_protocol = app_protocol
        # Configure the grid layout for better control
        self.grid_rowconfigure(1, weight=1)  # Text widget row should expand
        self.grid_columnconfigure(0, weight=1)  # Make sure it expands horizontally
        
        # Label at the top
        self.label = tk.Label(self, text="Logs:")
        self.label.grid(row=0, column=0, sticky="ew")  # Stick to top, expand horizontally
        
        # Text widget that expands in both directions with horizontal padding (padx)
        self.log = scrolledtext.ScrolledText(self)
        self.log.config(state=tk.DISABLED)
        self.log.grid(row=1, column=0, sticky="nsew", padx=10)  # Add horizontal padding with padx=10
        
        # Clear button positioned under the Text widget
        self.clear_button = tk.Button(self, text="Clear", command=self.clean_log)
        self.clear_button.grid(row=2, column=0)

    def update(self):
        self.log.config(state=tk.NORMAL)
        self.log.insert("end", self.app_protocol.getLogs())
        self.log.config(state=tk.DISABLED)
        self.log.see("end")

    def clean_log(self):
        self.log.config(state=tk.NORMAL)
        self.log.delete(1.0, 'end')
        self.log.config(state=tk.DISABLED)
