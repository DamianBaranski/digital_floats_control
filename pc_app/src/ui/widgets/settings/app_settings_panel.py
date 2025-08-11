import tkinter as tk
from tkinter import ttk, colorchooser
import json
from ui.widgets.settings.user_settings_panel import UserSettingsPanel
from src.core.datatypes.channel_settings import ChannelSettings
from ui.widgets.settings.channel_settings_table_panel import ChannelSettingsTablePanel
from ui.widgets.settings.auto_detect import AutoDetect
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT, SECTION_FONT

class UserSettingField(tk.Frame):
    def __init__(self, parent, name, input_type="entry"):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.name = name
        self.label = tk.Label(self, text=f"{name}:", bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
        self.label.pack(side="left", padx=5, pady=5)

        if input_type == "entry":
            self.entry = tk.Entry(self, bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                insertbackground=TEXT_COLOR)
            self.entry.pack(side="left", padx=5, pady=5, fill='x')
        elif input_type == "color":
            self.color_button = tk.Button(self, text="Choose Color", command=self.choose_color,
                                        bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                        activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
            self.color_button.pack(side="left", padx=5, pady=5)
            self.color_display = tk.Label(self, width=10, bg="white", relief="sunken")
            self.color_display.pack(side="left", padx=5, pady=5)

        self.input_type = input_type

    def choose_color(self):
        # Open color chooser dialog and set color
        color = colorchooser.askcolor(color=self.color_display.cget("bg"))[1]
        if color:
            self.color_display.config(bg=color)

    def set_value(self, value):
        if self.input_type == "entry":
            self.entry.delete(0, tk.END)
            self.entry.insert(0, value)
        elif self.input_type == "color":
            self.color_display.config(bg=value)

    def get_value(self):
        if self.input_type == "entry":
            return self.entry.get()
        elif self.input_type == "color":
            return self.color_display.cget("bg")

class AppSettingsPanel(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent, bg=DARK_BG)
        self.create_widgets()

    def create_widgets(self):
        # Create main frame with scrollbar
        self.main_canvas = tk.Canvas(self, bg=DARK_BG, highlightthickness=0)
        self.scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.main_canvas.yview)
        self.scrollable_frame = tk.Frame(self.main_canvas, bg=DARK_BG)

        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.main_canvas.configure(scrollregion=self.main_canvas.bbox("all"))
        )

        self.main_canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw", width=self.winfo_width())
        self.main_canvas.configure(yscrollcommand=self.scrollbar.set)

        # Pack the canvas and scrollbar
        self.main_canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")

        # Create title label
        self.title_label = tk.Label(self.scrollable_frame, text="Settings", font=HEADER_FONT,
                                  bg=DARK_BG, fg=TEXT_COLOR)
        self.title_label.pack(pady=(10, 20))

        # Create Channel Settings Section
        self.create_channel_settings_section()

        # Bind resize event
        self.bind('<Configure>', self.on_resize)

    def on_resize(self, event):
        # Update the width of the scrollable frame when the window is resized
        self.main_canvas.itemconfig(self.main_canvas.find_withtag("all")[0], width=event.width)

    def create_channel_settings_section(self):
        # Channel Settings Frame
        channel_frame = tk.Frame(self.scrollable_frame, bg=DARK_BG)
        channel_frame.pack(fill="x", padx=10, pady=(0, 20))

        # Channel Settings Title
        channel_title = tk.Label(channel_frame, text="Channel Settings", font=SECTION_FONT,
                               bg=DARK_BG, fg=TEXT_COLOR)
        channel_title.pack(anchor="w", pady=(0, 10))

        # Channel Settings Table
        self.channel_settings_table = ChannelSettingsTablePanel(channel_frame)
        self.channel_settings_table.pack(fill="x", pady=(0, 10))

        # Configure the treeview columns to be more compact
        if hasattr(self.channel_settings_table, 'tree'):
            self.channel_settings_table.tree.column("#0", width=50, minwidth=50)  # Channel column
            for col in self.channel_settings_table.tree["columns"]:
                self.channel_settings_table.tree.column(col, width=80, minwidth=80)

        # Channel Settings Buttons
        button_frame = tk.Frame(channel_frame, bg=DARK_BG)
        button_frame.pack(fill="x", pady=(0, 10))

        self.load_button = tk.Button(button_frame, text="Load", command=self.loadChannelSettings,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.load_button.pack(side="right", padx=2)

        self.save_button = tk.Button(button_frame, text="Save", command=self.saveChannelSettings,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.save_button.pack(side="right", padx=2)

        self.auto_detect_button = tk.Button(button_frame, text="Auto Detect", command=self.autoDetect,
                                          bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                          activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.auto_detect_button.pack(side="right", padx=2)

        self.help_button = tk.Button(button_frame, text="Help", command=self.channelHelp,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.help_button.pack(side="right", padx=2)

    def autoDetect(self):
        pass
        #auto_detect = AutoDetect(self.app_protocol, self.channel_settings_table)
        #auto_detect.start()

    def channelHelp(self):
        self.channel_settings_table.display_instructions()

    def loadChannelSettings(self):
        status = [False] * 6
        def callback(channel_settings, idx):
            status[idx] = True
            self.channel_settings_table.addData(idx)
            self.channel_settings_table.setData(idx, channel_settings) 
            if all(status):
                self.channel_settings_table.populate_treeview()

        #for i in range(6):
            #self.app_protocol.getChannelSettings(i, lambda data, idx=i: callback(data, idx))

    def saveChannelSettings(self):
        for i in range(6):
            channel_settings = self.channel_settings_table.getData(i)
            #self.app_protocol.updateChannelSettings(i, channel_settings)
            