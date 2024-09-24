try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk
from .table_widget import TableWidget

class MonitoringFrameWidget(tk.Frame):
    def __init__(self, parent):
        columns = [
            {"name": "Ina address", "width": 10, "type": "text"},
            {"name": "Pcf address", "width": 10, "type": "text"},
            {"name": "Voltage", "width": 10, "type": "text"},
            {"name": "Current", "width": 10, "type": "text"},
            {"name": "Up limit switch", "width": 10, "type": "bool"},
            {"name": "Down limit switch", "width": 10, "type": "bool"},
        ]
        tk.Frame.__init__(self, parent)
        self.label = tk.Label(self, text="Monitoring")
        self.table = TableWidget(self, columns)
        self.label.pack(side="top", fill="x")
        self.table.pack(side="top", fill="x", padx=20)
