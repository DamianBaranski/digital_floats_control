import tkinter as tk
from tkinter import colorchooser
from tkinter import ttk
from .user_settings_panel import UserSettingsPanel
from .channel_settings import ChannelSettings
from .channel_settings_table_panel import ChannelSettingsTablePanel
from .auto_detect import AutoDetect
from .ui_theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT, SECTION_FONT

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
    def __init__(self, parent, app_protocol):
        super().__init__(parent, bg=DARK_BG)
        self.app_protocol = app_protocol
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

        self.main_canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
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

        # Create LED Settings Section
        self.create_led_settings_section()

        # Create Communication Settings Section
        self.create_communication_settings_section()

    def create_channel_settings_section(self):
        # Channel Settings Frame
        channel_frame = tk.Frame(self.scrollable_frame, bg=DARK_BG)
        channel_frame.pack(fill="x", padx=20, pady=(0, 20))

        # Channel Settings Title
        channel_title = tk.Label(channel_frame, text="Channel Settings", font=SECTION_FONT,
                               bg=DARK_BG, fg=TEXT_COLOR)
        channel_title.pack(anchor="w", pady=(0, 10))

        # Channel Settings Table
        self.channel_settings_table = ChannelSettingsTablePanel(channel_frame)
        self.channel_settings_table.pack(fill="x", pady=(0, 10))

        # Channel Settings Buttons
        button_frame = tk.Frame(channel_frame, bg=DARK_BG)
        button_frame.pack(fill="x", pady=(0, 10))

        self.load_button = tk.Button(button_frame, text="Load", command=self.loadChannelSettings,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.load_button.pack(side="right", padx=5)

        self.save_button = tk.Button(button_frame, text="Save", command=self.saveChannelSettings,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.save_button.pack(side="right", padx=5)

        self.auto_detect_button = tk.Button(button_frame, text="Auto Detect", command=self.autoDetect,
                                          bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                          activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.auto_detect_button.pack(side="right", padx=5)

        self.help_button = tk.Button(button_frame, text="Help", command=self.channelHelp,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.help_button.pack(side="right", padx=5)

    def create_led_settings_section(self):
        # LED Settings Frame
        led_frame = tk.Frame(self.scrollable_frame, bg=DARK_BG)
        led_frame.pack(fill="x", padx=20, pady=(0, 20))

        # LED Settings Title
        led_title = tk.Label(led_frame, text="LED Settings", font=SECTION_FONT,
                           bg=DARK_BG, fg=TEXT_COLOR)
        led_title.pack(anchor="w", pady=(0, 10))

        # LED Settings Fields
        self.brightness = UserSettingField(led_frame, "Brightness")
        self.ldg_up_color = UserSettingField(led_frame, "LDG Up Color", input_type="color")
        self.ldg_down_color = UserSettingField(led_frame, "LDG Down Color", input_type="color")
        self.rudder_up_color = UserSettingField(led_frame, "Rudder Up Color", input_type="color")
        self.rudder_down_color = UserSettingField(led_frame, "Rudder Down Color", input_type="color")
        self.rudder_inactive_color = UserSettingField(led_frame, "Rudder Inactive Color", input_type="color")
        self.warning_color = UserSettingField(led_frame, "Warning Color", input_type="color")
        self.error_color = UserSettingField(led_frame, "Error Color", input_type="color")

        self.brightness.pack(fill="x", pady=5)
        self.ldg_up_color.pack(fill="x", pady=5)
        self.ldg_down_color.pack(fill="x", pady=5)
        self.rudder_up_color.pack(fill="x", pady=5)
        self.rudder_down_color.pack(fill="x", pady=5)
        self.rudder_inactive_color.pack(fill="x", pady=5)
        self.warning_color.pack(fill="x", pady=5)
        self.error_color.pack(fill="x", pady=5)

        # LED Settings Buttons
        button_frame = tk.Frame(led_frame, bg=DARK_BG)
        button_frame.pack(fill="x", pady=(10, 0))

        self.load_led_button = tk.Button(button_frame, text="Load", command=self.loadUserSettings,
                                       bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                       activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.load_led_button.pack(side="right", padx=5)

        self.save_led_button = tk.Button(button_frame, text="Save", command=self.saveUserSettings,
                                       bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                       activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.save_led_button.pack(side="right", padx=5)

        self.default_led_button = tk.Button(button_frame, text="Load Default", command=self.loadDefaultSettings,
                                          bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                          activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.default_led_button.pack(side="right", padx=5)

    def create_communication_settings_section(self):
        # Communication Settings Frame
        comm_frame = tk.Frame(self.scrollable_frame, bg=DARK_BG)
        comm_frame.pack(fill="x", padx=20, pady=(0, 20))

        # Communication Settings Title
        comm_title = tk.Label(comm_frame, text="Communication Settings", font=SECTION_FONT,
                            bg=DARK_BG, fg=TEXT_COLOR)
        comm_title.pack(anchor="w", pady=(0, 10))

        # Communication Settings Fields
        self.default_port = UserSettingField(comm_frame, "Default Port")
        self.baud_rate = UserSettingField(comm_frame, "Baud Rate")
        self.timeout = UserSettingField(comm_frame, "Timeout")
        self.retry_count = UserSettingField(comm_frame, "Retry Count")

        self.default_port.pack(fill="x", pady=5)
        self.baud_rate.pack(fill="x", pady=5)
        self.timeout.pack(fill="x", pady=5)
        self.retry_count.pack(fill="x", pady=5)

        # Communication Settings Buttons
        button_frame = tk.Frame(comm_frame, bg=DARK_BG)
        button_frame.pack(fill="x", pady=(10, 0))

        self.apply_comm_button = tk.Button(button_frame, text="Apply", command=self.apply_settings,
                                         bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                         activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.apply_comm_button.pack(side="right", padx=5)

    def updateUserSettings(self):
        # Set values from UserSettings to each field
        self.brightness.set_value(self.user_settings.get('brightness'))
        self.ldg_up_color.set_value(self.user_settings.get('ldg_up_color'))
        self.ldg_down_color.set_value(self.user_settings.get('ldg_down_color'))
        self.rudder_up_color.set_value(self.user_settings.get('rudder_up_color'))
        self.rudder_down_color.set_value(self.user_settings.get('rudder_down_color'))
        self.rudder_inactive_color.set_value(self.user_settings.get('rudder_inactive_color'))
        self.warning_color.set_value(self.user_settings.get('warning_color'))
        self.error_color.set_value(self.user_settings.get('error_color'))

    def loadUserSettings(self):
        def callback(data):
            if data != None:
                self.user_settings = data
            self.updateUserSettings()
    
        self.app_protocol.getUserSettings(callback)

    def saveUserSettings(self):
        # Save the current values from fields to UserSettings
        self.user_settings.set('brightness', self.brightness.get_value())
        self.user_settings.set('ldg_up_color', self.ldg_up_color.get_value())
        self.user_settings.set('ldg_down_color', self.ldg_down_color.get_value())
        self.user_settings.set('rudder_up_color', self.rudder_up_color.get_value())
        self.user_settings.set('rudder_down_color', self.rudder_down_color.get_value())
        self.user_settings.set('rudder_inactive_color', self.rudder_inactive_color.get_value())
        self.user_settings.set('warning_color', self.warning_color.get_value())
        self.user_settings.set('error_color', self.error_color.get_value())
        self.app_protocol.updateUserSettings(self.user_settings)

    def loadDefaultSettings(self):
        # Load default settings
        self.user_settings.setDefaults()
        self.updateUserSettings()

    def autoDetect(self):
        auto_detect = AutoDetect(self.app_protocol, self.channel_settings_table)
        auto_detect.start()

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

        for i in range(6):
            self.app_protocol.getChannelSettings(i, lambda data, idx=i: callback(data, idx))

    def saveChannelSettings(self):
        for i in range(6):
            channel_settings = self.channel_settings_table.getData(i)
            self.app_protocol.updateChannelSettings(i, channel_settings)

    def apply_settings(self):
        # Get settings from fields
        settings = {
            'default_port': self.default_port.get_value(),
            'baud_rate': self.baud_rate.get_value(),
            'timeout': self.timeout.get_value(),
            'retry_count': self.retry_count.get_value()
        }
        
        # Apply settings to app protocol
        if self.app_protocol:
            self.app_protocol.apply_settings(settings)
            