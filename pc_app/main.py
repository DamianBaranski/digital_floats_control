from com_port import ComPort
from protocol import AppProtocol
from widgets.com_port_widget import ComPortWidget
from widgets.status_frame_widget import StatusFrameWidget
from widgets.settings_frame_widget import SettingsFrameWidget
from widgets.monitoring_frame_widget import MonitoringFrameWidget
from widgets.logs_frame_widget import LogsFrameWidget
from widgets.ui_theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, FONT, HEADER_FONT

import tkinter as tk
from tkinter import ttk

class DetachableNotebook(ttk.Notebook):
    def __init__(self, master=None, tab_factories=None, **kw):
        super().__init__(master, **kw)
        self.tab_factories = tab_factories or {}
        self._active = None
        self._drag_data = {}
        self._detached_tabs = {}  # tab_text: (window, frame)
        self.bind('<ButtonPress-1>', self.on_tab_press, True)
        self.bind('<B1-Motion>', self.on_tab_motion, True)
        self.bind('<ButtonRelease-1>', self.on_tab_release, True)

    def on_tab_press(self, event):
        x, y = event.x, event.y
        elem = self.identify(x, y)
        if 'label' in elem:
            self._active = self.index(f"@{x},{y}")
            self._drag_data = {'x': x, 'y': y}

    def on_tab_motion(self, event):
        pass

    def on_tab_release(self, event):
        if self._active is not None:
            x_root, y_root = event.x_root, event.y_root
            tab_id = self.tabs()[self._active]
            tab_text = self.tab(self._active, 'text')
            nb_x = self.winfo_rootx()
            nb_y = self.winfo_rooty()
            nb_w = self.winfo_width()
            nb_h = self.winfo_height()
            if not (nb_x <= x_root <= nb_x + nb_w and nb_y <= y_root <= nb_y + nb_h):
                self.detach_tab(self._active, tab_text, x_root, y_root)
            self._active = None
            self._drag_data = {}

    def detach_tab(self, tab_index, tab_text, x_root, y_root):
        # Remove the tab and destroy its frame
        tab_id = self.tabs()[tab_index]
        tab_frame = self.nametowidget(tab_id)
        self.forget(tab_index)
        tab_frame.destroy()
        # Create a new window and new frame for the tab
        new_win = tk.Toplevel(self)
        new_win.title(tab_text)
        new_win.configure(bg=DARK_BG)
        frame = tk.Frame(new_win, bg=DARK_BG)
        frame.pack(fill='both', expand=True)
        # Create the tab content using the factory
        widget = self.tab_factories[tab_text](frame)
        widget.pack(fill='both', expand=True)
        btn = tk.Button(new_win, text="Reattach Tab", command=lambda: self.reattach_tab(tab_text, new_win, frame),
                        bg=DARKER_BG, fg=TEXT_COLOR, font=FONT, activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        btn.pack(side='bottom', fill='x')
        new_win.geometry(f"600x400+{x_root}+{y_root}")
        self._detached_tabs[tab_text] = (new_win, frame)
        new_win.protocol("WM_DELETE_WINDOW", lambda: self.reattach_tab(tab_text, new_win, frame))

    def reattach_tab(self, tab_text, win, frame):
        if tab_text in self._detached_tabs:
            win.withdraw()
            win.destroy()
            frame.destroy()
            # Recreate the tab in the notebook
            new_frame = tk.Frame(self, bg=DARK_BG)
            widget = self.tab_factories[tab_text](new_frame)
            widget.pack(fill='both', expand=True)
            self.add(new_frame, text=tab_text)
            self.select(new_frame)
            del self._detached_tabs[tab_text]

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
            for frame in self.ui_tabs.winfo_children():
                if self.ui_tabs.tab(frame, option='text') == tab_name:
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
