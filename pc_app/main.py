from com_port import ComPort
from protocol import AppProtocol
from widgets.com_port_widget import ComPortWidget
from widgets.status_frame_widget import StatusFrameWidget
from widgets.settings_frame_widget import SettingsFrameWidget
from widgets.monitoring_frame_widget import MonitoringFrameWidget
from widgets.logs_frame_widget import LogsFrameWidget

try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class DigitalFloatsApp(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent)
        parent.title("Digital Floats App")

        # Initialize the ComPort
        self.comport = ComPort(self.on_connected)
        self.app_protocol = AppProtocol(self.comport)

        # Create a frame for the left column (including ComPortWidget and other widgets, if needed)
        self.left_frame = tk.Frame(self)
        self.left_frame.grid(row=0, column=0, sticky="ns")

        # Place the ComPortWidget at the top of the left column
        self.ui_port = ComPortWidget(self.left_frame, self.comport)
        self.ui_port.pack(padx=10, pady=10, side=tk.TOP, fill=tk.X)

        # Create the notebook for tabs
        self.ui_tabs = Notebook(self)
        self.ui_tabs.grid(row=0, column=1, sticky="nsew")  # Fill horizontally and vertically

        # Create and add tabs to the notebook
        self.ui_status_tab = StatusFrameWidget(self.ui_tabs, self.app_protocol)
        self.ui_settings_tab = SettingsFrameWidget(self.ui_tabs, self.app_protocol)
        self.ui_monitoring_tab = MonitoringFrameWidget(self.ui_tabs, self.app_protocol)
        self.ui_logs_tab = LogsFrameWidget(self.ui_tabs, self.app_protocol)

        self.ui_tabs.add(self.ui_status_tab, text="Status")
        self.ui_tabs.add(self.ui_settings_tab, text="Settings")
        self.ui_tabs.add(self.ui_monitoring_tab, text="Monitoring")
        self.ui_tabs.add(self.ui_logs_tab, text="Logs")

        # Create and place the status bar at the bottom
        self.ui_status_bar = Label(parent, relief=tk.SUNKEN, anchor="w")
        self.ui_status_bar.grid(row=1, column=0, columnspan=1, sticky="ew")

        # Configure the grid for resizing behavior
        self.grid_rowconfigure(0, weight=1)  # Make row 0 expandable in DigitalFloatsApp
        self.grid_columnconfigure(1, weight=1)  # Make column 1 expandable in DigitalFloatsApp

        self.update()

    def submit(self):
        self.ui_status_bar.config(text="my_text")
        pass

    def update(self):
        self.ui_port.update()
        self.ui_logs_tab.update()
        # call this function again in one second
        self.after(1000, self.update)
        
    def on_connected(self, status):
        if status:
            self.ui_status_tab.update()

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("800x950")
    app = DigitalFloatsApp(root)
    app.grid(row=0, column=0, sticky="nsew")
    root.grid_rowconfigure(0, weight=1)  # Make sure the main frame can expand
    root.grid_columnconfigure(0, weight=1)
    root.mainloop()
