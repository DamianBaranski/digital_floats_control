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
        self.label = tk.Label(self, text="Port:")
        self.label.pack(side="top", fill="x")
