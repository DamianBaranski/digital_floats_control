try:
    # python 3.x
    import tkinter as tk
    from tkinter.ttk import *
    import tkinter.messagebox
except ImportError:
    # python 2.x
    import Tkinter as tk
    import Tkinter.messagebox

from .ui_theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT

class ComPortWidget(tk.Frame):
    def __init__(self, parent, com_port):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.label = tk.Label(self, text="Port:", bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
        self.port_list = Combobox(self, font=FONT)
        self.button = tk.Button(self, text="Open", command=self.button_callback,
                              bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                              activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.label.pack(side="top", fill=tk.X)
        self.port_list.pack(side="top", fill=tk.X)
        self.button.pack(side="top", fill=tk.X)
        self.com_port = com_port
        self.updatePortsList()

    def button_callback(self):
        try:
            if self.com_port.isOpen():
                self.com_port.close()
            else:
                self.com_port.open(self.port_list.get())
        except Exception as e:
            tk.messagebox.showerror(message=e)

    def updatePortsList(self):
        self.port_list['values']=self.com_port.getPortsList()

    def update(self):
        self.updatePortsList()
        if self.com_port.isOpen():
            self.port_list['state'] = "disabled"
            self.button.configure(text="Close")
        else:
            self.port_list['state'] = "normal"
            self.button.configure(text="Open")

