try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class StatusFrameWidget(tk.Frame):
    def __init__(self, parent, app_protocol):
        tk.Frame.__init__(self, parent)
        self.app_protocol = app_protocol
        self.ver_label = tk.Label(self, text="Firmware ver:")
        self.ver_value = tk.Label(self, text="N/A")
        self.ver_label.grid(padx=10, row=1, column=1)
        self.ver_value.grid(row=1, column=2)
        

    def update(self):
        version = self.app_protocol.getVersion(lambda version: self.ver_value.config(text = version))
        
