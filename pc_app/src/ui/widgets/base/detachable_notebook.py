import tkinter as tk
from tkinter import ttk
from src.ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, FONT

class DetachableNotebook(ttk.Notebook):
    """
    A ttk.Notebook subclass that allows tabs to be detached into separate windows and reattached.
    """
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
        # Get parent window size and position
        root = self.winfo_toplevel()
        root.update_idletasks()
        width = root.winfo_width()
        height = root.winfo_height()
        x = root.winfo_rootx()
        y = root.winfo_rooty()
        new_win.geometry(f"{width}x{height}+{x}+{y}")
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