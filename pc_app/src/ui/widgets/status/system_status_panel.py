from src.ui.widgets.status.firmware_upload import FirmwareUpload
from src.ui.widgets.status.release import Release
import multiprocessing
from ui.widgets.status.hardware_status_canvas import HardwareStatusCanvas
from ui.widgets.status.log_table_panel import LogTablePanel
from ui.widgets.status.quick_status_panel import QuickStatusPanel
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT
import os
import logging
from typing import List, Optional

try:
    # python 3.x
    import tkinter as tk
    from tkinter import ttk
except ImportError:
    # python 2.x
    import Tkinter as tk

class SystemStatusPanel(tk.Frame):
    def __init__(self, parent, device_client=None):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.device_client = device_client
        self.ver_label = tk.Label(self, text="Firmware ver:", bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
        self.ver_value = tk.Label(self, text="N/A", bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
        self.firmware_upload_button = tk.Button(self, text="Update firmware", command=self.firmware_upload,
                                              bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                              activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.ver_label.grid(padx=10, row=1, column=1)
        self.ver_value.grid(row=1, column=2)
        #self.firmware_upload_button.grid(row=2, column=1)
        self.updating = multiprocessing.Value('b', False)
        self.version = None

        # Main horizontal split: left = status panel + log, right = quick status
        main_paned = tk.PanedWindow(self, orient=tk.HORIZONTAL, sashrelief=tk.RAISED, bg=DARK_BG)
        main_paned.grid(row=3, column=1, columnspan=2, sticky="nsew", padx=10, pady=10)
        self.grid_rowconfigure(3, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # Left: vertical split (status panel + log)
        left_paned = tk.PanedWindow(main_paned, orient=tk.VERTICAL, sashrelief=tk.RAISED, bg=DARK_BG)
        base_dir = os.path.dirname(os.path.abspath(__file__))
        layout_json_path = os.path.join(base_dir, "resources", "panel_layout.json")
        self.status_panel = HardwareStatusCanvas(left_paned, layout_json_path)
        #self.status_panel.animate_indicators_edit_mode(interval=1000)
        self.status_panel.pack(fill="both", expand=True)
        left_paned.add(self.status_panel)
        self.log_widget = LogTablePanel(left_paned)
        left_paned.add(self.log_widget)
        main_paned.add(left_paned)

        # Right: quick status
        self.quick_status = QuickStatusPanel(main_paned, self.device_client)
        main_paned.add(self.quick_status)

    def setStatus(self, status):
        """Update status data from StatusData protocol."""
        if hasattr(self, 'quick_status') and status:
            self.quick_status.updateStatusData(status)
        
        
    def update(self):
        if self.device_client and self.device_client.isConnected():
            # Load firmware info and status data via QuickStatusPanel
            if hasattr(self, 'quick_status'):
                self.quick_status.loadFirmwareInfo()
                self.quick_status.loadStatusData()
        #    
        #if self.quick_status.remote_enabled:
        #    self.app_protocol.simulate(lambda response: None, 0, 0, 0)
            
        #for i in range(6):
        #    self.app_protocol.getMonitoringData(i, lambda data, idx=i: self._update_callback(data, idx))

    def _update_callback(self, data, idx):
        pass
        #self.status_panel.
        
    def updateVersion(self, version):
        if self.updating.value == True:
            return
        
        self.ver_value.config(text = version)
        if self.version != version:
            self.version = version
            #check update
            try:
                r = Release("DamianBaranski", "digital_floats_control")
                release_ver = r.getLatestTag()
                version = version[version.find('v')+1:]
                print(f"Release ver:{release_ver} vs installed ver:{self.version}")
                if release_ver > version:
                    tk.messagebox.showinfo(title=None, message=f"New firmware update available: {release_ver}")
            except:
                pass
            
    def firmware_upload(self):
        with self.updating.get_lock():
            self.updating.value = True
        #uploader = FirmwareUpload(self.app_protocol, None)
        #uploader.start(callback=self.on_upgrade_finished)
    
    def on_upgrade_finished(self, result):
        with self.updating.get_lock():
            self.updating.value = False 
