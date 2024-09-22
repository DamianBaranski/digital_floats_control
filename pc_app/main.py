from com_port import ComPort
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
        self.ui_statusBar=Label(root, relief=tk.SUNKEN, anchor="w")
        self.ui_statusBar.pack(side=tk.BOTTOM, fill=tk.X)
        self.ui_port = ComPortWidget(self)
        self.ui_tabs = Notebook(self)

        self.ui_port.pack(pady=0, side=tk.LEFT, expand=False)
        self.ui_tabs.pack(fill=tk.BOTH, side=tk.LEFT, expand=True)

        self.ui_status_tab = StatusFrameWidget(self.ui_tabs)
        self.ui_settings_tab = SettingsFrameWidget(self.ui_tabs)
        self.ui_monitoring_tab = MonitoringFrameWidget(self.ui_tabs)
        self.ui_logs_tab = LogsFrameWidget(self.ui_tabs)

        self.ui_tabs.add(self.ui_status_tab, text = "Status")
        self.ui_tabs.add(self.ui_settings_tab, text = "Settings")
        self.ui_tabs.add(self.ui_monitoring_tab, text = "Monitoring")
        self.ui_tabs.add(self.ui_logs_tab, text = "Logs")

        self.grid_columnconfigure(0, weight=1)
        self.grid_rowconfigure(2, weight=1)

        self.comport = ComPort()
        self.updateComPorts()

    def updateComPorts(self):
        self.ui_port.set_com_ports(self.comport.getPortsList())

    def submit(self):
        self.ui_statusBar.config(text = "my_text")
        pass

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("400x400")
    DigitalFloatsApp(root).place(x=0, y=0, relwidth=1, relheight=1)
    root.mainloop()
