try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
except ImportError:
    # python 2.x
    import Tkinter as tk

class ComPortWidget(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent)
        self.label = tk.Label(self, text="Port:")
        self.port_list = Combobox(self)
        self.button = tk.Button(self, text="Open")
        self.label.pack(side="top", fill=tk.X)
        self.port_list.pack(side="top", fill=tk.X)
        self.button.pack(side="top", fill=tk.X)

    def set_com_ports(self, ports):
        print(ports)
        self.port_list['values']=ports

