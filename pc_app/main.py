from com_port import ComPort
from protocol import AppProtocol
from widgets.com_port_widget import ComPortWidget
from widgets.status_frame_widget import StatusFrameWidget
from widgets.settings_frame_widget import SettingsFrameWidget
from widgets.monitoring_frame_widget import MonitoringFrameWidget
from widgets.logs_frame_widget import LogsFrameWidget
from widgets.ui_theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, FONT, HEADER_FONT
from widgets.detachable_notebook import DetachableNotebook

import tkinter as tk
from tkinter import ttk

class DigitalFloatsApp(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.parent = parent
        self.parent.title("Digital Floats App")
        self.parent.configure(bg=DARK_BG)

        # Configure ttk styles
        style = ttk.Style()
        style.theme_use('default')

        # Notebook
        style.configure("TNotebook", background=DARK_BG, borderwidth=0)
        style.configure("TNotebook.Tab", background=DARKER_BG, foreground=TEXT_COLOR, font=FONT, padding=[10, 2])
        style.map("TNotebook.Tab",
                  background=[("selected", BORDER_COLOR)],
                  foreground=[("selected", TEXT_COLOR)])

        # Frame and Label
        style.configure("TFrame", background=DARK_BG)
        style.configure("TLabel", background=DARK_BG, foreground=TEXT_COLOR, font=FONT)

        # Button
        style.configure("TButton", background=DARKER_BG, foreground=TEXT_COLOR, font=FONT, borderwidth=1)
        style.map("TButton",
                  background=[("active", BORDER_COLOR)],
                  foreground=[("active", TEXT_COLOR)])

        # Entry
        style.configure("TEntry", fieldbackground=DARKER_BG, foreground=TEXT_COLOR, font=FONT, insertcolor=TEXT_COLOR)

        # Combobox
        style.configure("TCombobox", fieldbackground=DARKER_BG, background=DARKER_BG, foreground=TEXT_COLOR, font=FONT)
        style.map("TCombobox",
                  fieldbackground=[("readonly", DARKER_BG)],
                  selectbackground=[("readonly", BORDER_COLOR)],
                  selectforeground=[("readonly", TEXT_COLOR)])

        # Scrollbar
        style.configure("TScrollbar", background=DARKER_BG, troughcolor=DARK_BG, bordercolor=BORDER_COLOR, arrowcolor=TEXT_COLOR)
        style.map("TScrollbar",
                  background=[("active", BORDER_COLOR)],
                  arrowcolor=[("active", TEXT_COLOR)])

        # Initialize the ComPort
        self.comport = ComPort(self.on_connected)
        self.app_protocol = AppProtocol(self.comport)

        # Create a frame for the left column (including ComPortWidget and other widgets, if needed)
        self.left_frame = tk.Frame(self, bg=DARK_BG)
        self.left_frame.grid(row=0, column=0, sticky="ns")

        # Place the ComPortWidget at the top of the left column
        self.ui_port = ComPortWidget(self.left_frame, self.comport)
        self.ui_port.pack(padx=10, pady=10, side=tk.TOP, fill=tk.X)

        # Add a vertical separator between left and right columns
        self.vert_separator = ttk.Separator(self, orient='vertical')
        self.vert_separator.grid(row=0, column=1, sticky='ns')

        # Tab factories for robust detach/reattach
        tab_factories = {
            "Status": lambda parent: StatusFrameWidget(parent, self.app_protocol),
            "Settings": lambda parent: SettingsFrameWidget(parent, self.app_protocol),
            "Monitoring": lambda parent: MonitoringFrameWidget(parent, self.app_protocol),
            "Logs": lambda parent: LogsFrameWidget(parent, self.app_protocol),
        }

        # Create the detachable notebook for tabs
        self.ui_tabs = DetachableNotebook(self, tab_factories=tab_factories)
        self.ui_tabs.grid(row=0, column=2, sticky="nsew")  # Fill horizontally and vertically

        # Create and add tabs to the notebook using factories
        for tab_name in ["Status", "Settings", "Monitoring", "Logs"]:
            frame = tk.Frame(self.ui_tabs, bg=DARK_BG)
            widget = tab_factories[tab_name](frame)
            widget.pack(fill='both', expand=True)
            self.ui_tabs.add(frame, text=tab_name)

        # Create and place the status bar at the bottom
        self.ui_status_bar = ttk.Label(parent, relief=tk.SUNKEN, anchor="w", background=DARKER_BG, foreground=TEXT_COLOR, font=FONT)
        self.ui_status_bar.grid(row=1, column=0, columnspan=3, sticky="ew")

        # Configure the grid for resizing behavior
        self.grid_rowconfigure(0, weight=1)  # Make row 0 expandable in DigitalFloatsApp
        self.grid_columnconfigure(2, weight=1)  # Make column 2 expandable in DigitalFloatsApp
        
        # Bind the window close event
        parent.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.update()

    def on_closing(self):
        self.comport.close()
        self.comport.stop()
        self.parent.destroy()   # Destroy the window
        
    def submit(self):
        self.ui_status_bar.config(text="my_text")
        pass

    def update(self):
        # Get the index of the currently selected tab
        selected_tab_index = self.ui_tabs.index(self.ui_tabs.select())
        tab_names = ["Status", "Settings", "Monitoring", "Logs"]
        if 0 <= selected_tab_index < len(tab_names):
            tab_name = tab_names[selected_tab_index]
            for tab_id in self.ui_tabs.tabs():
                frame = self.ui_tabs.nametowidget(tab_id)
                if self.ui_tabs.tab(tab_id, option='text') == tab_name:
                    for child in frame.winfo_children():
                        if hasattr(child, 'update'):
                            child.update()
        self.ui_port.update()
        self.after(100, self.update)
        
    def on_connected(self, status):
        if status:
            for frame in self.ui_tabs.winfo_children():
                if self.ui_tabs.tab(frame, option='text') == "Status":
                    for child in frame.winfo_children():
                        if hasattr(child, 'update'):
                            child.update()

if __name__ == "__main__":
    root = tk.Tk()
    root.geometry("1400x900")
    root.configure(bg=DARK_BG)  # Set root window background
    app = DigitalFloatsApp(root)
    app.grid(row=0, column=0, sticky="nsew")
    root.grid_rowconfigure(0, weight=1)  # Make sure the main frame can expand
    root.grid_columnconfigure(0, weight=1)
    root.mainloop()
