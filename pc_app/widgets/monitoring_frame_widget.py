try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk
from .monitoring_table import MonitoringTable
import time

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
        self.data_ready = [True, True, True, True, True, True]
        self.data_request_time = time.time()
        self.timeout = 60

    def update(self):
        if all(self.data_ready) or self.data_request_time + self.timeout < time.time():
            self.data_request_time = time.time()
            self.data_ready = [False] * len(self.data_ready)
            self.table.populate_treeview()
            for i in range(6):
                self.protocol.getMonitoringData(i, lambda data, idx=i: self._update_callback(data, idx))

    def _update_callback(self, data, idx):
        self.table.setData(idx, data)
        self.data_ready[idx] = True