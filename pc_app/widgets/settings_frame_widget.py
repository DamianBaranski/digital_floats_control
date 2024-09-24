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
        self.channels_settings_label = tk.Label(self, text="Channels settings")
        self.table = TableWidget(self, columns)
        self.channel_settings_load = tk.Button(self, text="Load")
        self.channel_settings_save = tk.Button(self, text="Save")
        self.channel_settings_autodetect = tk.Button(self, text="Autodetect")

        self.user_settings = tk.Label(self, text="User settings")
        self.channels_settings_label.pack(side="top", fill="x")
        self.table.pack(side="top", fill="x")
        self.channel_settings_load.pack(side="top", fill="x")
        self.channel_settings_save.pack(side="top", fill="x")
        self.channel_settings_autodetect.pack(side="top", fill="x")
        self.user_settings.pack(side="top", fill="x")

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("600x400")  # Set window size
    SettingsFrameWidget(root).pack(fill="both", expand=True, padx=10)
    root.mainloop()
