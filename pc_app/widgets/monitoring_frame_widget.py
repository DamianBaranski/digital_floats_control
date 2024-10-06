try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk
from .monitoring_table import MonitoringTable

class MonitoringFrameWidget(tk.Frame):
    def __init__(self, parent, protocol):
        tk.Frame.__init__(self, parent)
        self.protocol = protocol
        frame = tk.Frame(self)
        frame.pack(side="top", fill="x", padx=10, pady=10)
        
        self.label = tk.Label(frame, text="Monitoring")
        self.table = MonitoringTable(self)
        self.label.pack(side="top", fill="x")
        self.table.pack(side="top", fill="x", padx=20)

    def update(self):
        for i in range(6):
            data = self.protocol.getMonitoringData(i)
            print(data)
            self.table.setData(i, data)