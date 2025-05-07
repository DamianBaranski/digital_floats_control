from .firmware_upload import FirmwareUpload
from .release import Release
import multiprocessing
from .status_panel_composite_widget import StatusPanelCompositeWidget
from .log_widget import LogWidget
from .quick_status_widget import QuickStatusWidget
import os

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
        self.firmware_upload_button = tk.Button(self, text="Update firmware", command=self.firmware_upload)
        self.ver_label.grid(padx=10, row=1, column=1)
        self.ver_value.grid(row=1, column=2)
        #self.firmware_upload_button.grid(row=2, column=1)
        self.updating = multiprocessing.Value('b', False)
        self.version = None

        # Main horizontal split: left = status panel + log, right = quick status
        main_paned = tk.PanedWindow(self, orient=tk.HORIZONTAL, sashrelief=tk.RAISED)
        main_paned.grid(row=3, column=1, columnspan=2, sticky="nsew", padx=10, pady=10)
        self.grid_rowconfigure(3, weight=1)
        self.grid_columnconfigure(1, weight=1)

        # Left: vertical split (status panel + log)
        left_paned = tk.PanedWindow(main_paned, orient=tk.VERTICAL, sashrelief=tk.RAISED)
        base_dir = os.path.dirname(os.path.abspath(__file__))
        layout_json_path = os.path.join(base_dir, "res", "panel_layout.json")
        self.status_panel = StatusPanelCompositeWidget(left_paned, layout_json_path)
        self.status_panel.animate_indicators_edit_mode(interval=1000)
        left_paned.add(self.status_panel)
        self.log_widget = LogWidget(left_paned)
        left_paned.add(self.log_widget)
        main_paned.add(left_paned)

        # Right: quick status
        self.quick_status = QuickStatusWidget(main_paned)
        main_paned.add(self.quick_status)

    def update(self):
        if not self.app_protocol.uart.isOpen():
            self.updateVersion('N/A')
            return
        
        if self.updating.value == False:
            self.app_protocol.getVersion(self.updateVersion)
        
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
        uploader = FirmwareUpload(self.app_protocol, None)
        uploader.start(callback=self.on_upgrade_finished)
    
    def on_upgrade_finished(self, result):
        with self.updating.get_lock():
            self.updating.value = False 
