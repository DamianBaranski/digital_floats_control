import tkinter as tk
from tkinter import ttk
import json
import os
import struct
from typing import List, Optional
from ui.widgets.monitoring.monitoring_table_panel import MonitoringTablePanel
from core.protocol import requests
import time
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT

class MonitoringPanel(tk.Frame):
    def __init__(self, parent, device_client=None):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.device_client = device_client
        frame = tk.Frame(self, bg=DARK_BG)
        frame.pack(side="top", fill="x", padx=10, pady=10)
        
        self.label = tk.Label(frame, text="Monitoring", bg=DARK_BG, fg=TEXT_COLOR, font=HEADER_FONT)
        self.table = MonitoringTablePanel(self)
        self.label.pack(side="top", fill="x")
        self.table.pack(side="top", fill="x", padx=20)
        self.data_ready = [True, True, True, True, True, True]
        self.data_request_time = time.time()
        self.timeout = 60
        self.voltage = None  # Store voltage from StatusData

    def update(self):
        if not self.device_client or not self.device_client.isConnected():
            return
            
        if all(self.data_ready) or self.data_request_time + self.timeout < time.time():
            self.data_request_time = time.time()
            self.data_ready = [False] * len(self.data_ready)
            self.table.populate_treeview()
            
            # First, request StatusData to get voltage (one request for all channels)
            self.device_client.command(requests.StatusDataRequest(), self._status_data_callback)
            
            # Then request monitoring data for all 6 channels
            for i in range(6):
                self.device_client.command(requests.MonitoringDataRequest(i), 
                                         lambda data, idx=i: self._update_callback(data, idx))

    def _update_callback(self, data, idx):
        if data:
            # Set voltage from StatusData if available
            if self.voltage is not None:
                data.setVoltage(self.voltage)
            self.table.setData(idx, data)
            self.data_ready[idx] = True
            # Update the treeview immediately when data is received
            self.table.populate_treeview()
    
    def _status_data_callback(self, status_data):
        """Callback for StatusData to get voltage (one request for all channels)."""
        if status_data:
            voltage = status_data.get_power_voltage()
            if voltage is not None:
                self.voltage = voltage
                # Voltage will be applied to monitoring data when it arrives in _update_callback
    
    def loadMonitoringData(self):
        """Load monitoring data for all channels."""
        if not self.device_client or not self.device_client.isConnected():
            return
        
        self.data_ready = [False] * 6
        
        # First, request StatusData to get voltage (one request for all channels)
        self.device_client.command(requests.StatusDataRequest(), self._status_data_callback)
        
        # Then request monitoring data for all 6 channels
        for i in range(6):
            self.device_client.command(requests.MonitoringDataRequest(i), 
                                     lambda data, idx=i: self._update_callback(data, idx))