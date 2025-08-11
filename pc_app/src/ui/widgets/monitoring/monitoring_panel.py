import tkinter as tk
from tkinter import ttk
import json
import os
import struct
from typing import List, Optional
from ui.widgets.monitoring.monitoring_table_panel import MonitoringTablePanel
import time
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT

class MonitoringPanel(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        frame = tk.Frame(self, bg=DARK_BG)
        frame.pack(side="top", fill="x", padx=10, pady=10)
        
        self.label = tk.Label(frame, text="Monitoring", bg=DARK_BG, fg=TEXT_COLOR, font=HEADER_FONT)
        self.table = MonitoringTablePanel(self)
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
            #for i in range(6):
                #self.protocol.getMonitoringData(i, lambda data, idx=i: self._update_callback(data, idx))

    def _update_callback(self, data, idx):
        self.table.setData(idx, data)
        self.data_ready[idx] = True