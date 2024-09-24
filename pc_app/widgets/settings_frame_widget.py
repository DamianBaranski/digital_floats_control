try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk
from .table_widget import TableWidget

class SettingsFrameWidget(tk.Frame):
    def __init__(self, parent):
        columns = [
            {"name": "Bridge", "width": 10, "type": "bool"},
            {"name": "Inverse motor", "width": 10, "type": "bool"},
            {"name": "Inverse up limit switch", "width": 10, "type": "bool"},
            {"name": "Inverse down limit switch", "width": 10, "type": "bool"},
            {"name": "Inverse limit switch", "width": 10, "type": "bool"},
            {"name": "Rudder", "width": 10, "type": "bool"},
            {"name": "Ina address", "width": 100, "type": "int"},
            {"name": "Ina calibration", "width": 100, "type": "int"},
            {"name": "Pcf address", "width": 100, "type": "int"},
            {"name": "Pcf channel", "width": 100, "type": "int"},
            {"name": "Max voltage", "width": 100, "type": "int"},
            {"name": "Min voltage", "width": 100, "type": "int"},
            {"name": "Max current", "width": 100, "type": "int"},
            {"name": "Min current", "width": 100, "type": "int"},
        ]
  
        tk.Frame.__init__(self, parent)

        # Frame for Channel Settings
        channel_frame = tk.Frame(self)
        channel_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.channels_settings_label = tk.Label(channel_frame, text="Channels settings")
        self.channels_settings_label.pack(side="top", fill="x")
        
        self.table = TableWidget(channel_frame, columns)
        self.table.pack(side="top", fill="x")

        # Frame for Buttons (Load, Save, Autodetect)
        button_frame = tk.Frame(channel_frame)
        button_frame.pack(side="top", fill="x", pady=10)

        self.channel_settings_load = tk.Button(button_frame, text="Load")
        self.channel_settings_load.pack(side="left", expand=True)

        self.channel_settings_save = tk.Button(button_frame, text="Save")
        self.channel_settings_save.pack(side="left", expand=True)

        self.channel_settings_autodetect = tk.Button(button_frame, text="Autodetect")
        self.channel_settings_autodetect.pack(side="left", expand=True)

        # Frame for User Settings
        user_frame = tk.Frame(self)
        user_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.user_settings = tk.Label(user_frame, text="User settings")
        self.user_settings.pack(side="top", fill="x")

