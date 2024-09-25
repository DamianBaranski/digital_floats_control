import tkinter as tk
from tkinter import colorchooser
from tkinter import ttk
from .table_widget import TableWidget
from .user_settings import UserSettings

class UserSettingField(tk.Frame):
    def __init__(self, parent, name, input_type="entry"):
        tk.Frame.__init__(self, parent)
        self.name = name
        self.label = tk.Label(self, text=f"{name}:")
        self.label.pack(side="left", padx=5, pady=5)

        if input_type == "entry":
            self.entry = tk.Entry(self)
            self.entry.pack(side="left", padx=5, pady=5, fill='x')
        elif input_type == "color":
            self.color_button = tk.Button(self, text="Choose Color", command=self.choose_color)
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

class SettingsFrameWidget(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent)
        self.user_settings = UserSettings()
        # Frame for Channel Settings
        channel_frame = tk.Frame(self)
        channel_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.channels_settings_label = tk.Label(channel_frame, text="Channels Settings", font=("Helvetica", 12, "bold"))
        self.channels_settings_label.pack(side="top", fill="x")

        # Assume TableWidget exists to handle channel settings
        columns = [
            {"name": "Bridge", "width": 10, "type": "bool"},
            {"name": "Inverse Motor", "width": 10, "type": "bool"},
            {"name": "Inverse Up Limit Switch", "width": 10, "type": "bool"},
            {"name": "Inverse Down Limit Switch", "width": 10, "type": "bool"},
            {"name": "Inverse Limit Switch", "width": 10, "type": "bool"},
            {"name": "Rudder", "width": 10, "type": "bool"},
            {"name": "INA Address", "width": 100, "type": "int"},
            {"name": "INA Calibration", "width": 100, "type": "int"},
            {"name": "PCF Address", "width": 100, "type": "int"},
            {"name": "PCF Channel", "width": 100, "type": "int"},
            {"name": "Max Voltage", "width": 100, "type": "int"},
            {"name": "Min Voltage", "width": 100, "type": "int"},
            {"name": "Max Current", "width": 100, "type": "int"},
            {"name": "Min Current", "width": 100, "type": "int"},
        ]

        self.table = TableWidget(channel_frame, columns)
        self.table.pack(side="top", fill="x")

        # Frame for Buttons (Load, Save, Autodetect)
        button_frame = tk.Frame(channel_frame)
        button_frame.pack(side="top", fill="x", pady=10)

        self.channel_settings_load = tk.Button(button_frame, text="Load")
        self.channel_settings_load.pack(side="right", padx=5)

        self.channel_settings_save = tk.Button(button_frame, text="Save")
        self.channel_settings_save.pack(side="right", padx=5)

        self.channel_settings_autodetect = tk.Button(button_frame, text="Autodetect")
        self.channel_settings_autodetect.pack(side="right", padx=5)

        # Frame for User Settings
        user_frame = tk.Frame(self)
        user_frame.pack(side="top", fill="x", padx=10, pady=10)

        self.user_settings_label = tk.Label(user_frame, text="User Settings", font=("Helvetica", 12, "bold"))
        self.user_settings_label.pack(side="top", fill="x")

        # Create a frame to group user setting fields
        user_settings_group = tk.Frame(user_frame)
        user_settings_group.pack(side="top", fill="x", pady=(5, 10))

        # Add user setting fields
        self.brightness = UserSettingField(user_settings_group, "Brightness")
        self.ldg_up_color = UserSettingField(user_settings_group, "LDG Up Color", input_type="color")
        self.ldg_down_color = UserSettingField(user_settings_group, "LDG Down Color", input_type="color")
        self.rudder_up_color = UserSettingField(user_settings_group, "Rudder Up Color", input_type="color")
        self.rudder_down_color = UserSettingField(user_settings_group, "Rudder Down Color", input_type="color")
        self.rudder_inactive_color = UserSettingField(user_settings_group, "Rudder Inactive Color", input_type="color")
        self.warning_color = UserSettingField(user_settings_group, "Warning Color", input_type="color")
        self.error_color = UserSettingField(user_settings_group, "Error Color", input_type="color")


        self.brightness.pack(side="top", fill="x", pady=5)
        self.ldg_up_color.pack(side="top", fill="x", pady=5)
        self.ldg_down_color.pack(side="top", fill="x", pady=5)
        self.rudder_up_color.pack(side="top", fill="x", pady=5)
        self.rudder_down_color.pack(side="top", fill="x", pady=5)
        self.rudder_inactive_color.pack(side="top", fill="x", pady=5)
        self.warning_color.pack(side="top", fill="x", pady=5)
        self.error_color.pack(side="top", fill="x", pady=5)

        # Add Save and load buttons
        button_frame = tk.Frame(self)
        button_frame.pack(side="top", fill="x", padx=10, pady=(5, 10))

        self.load_button = tk.Button(button_frame, text="Load", command=self.loadUserSettings)
        self.load_button.pack(side="right", padx=5)

        self.save_button = tk.Button(button_frame, text="Save", command=self.saveUserSettings)
        self.save_button.pack(side="right", padx=5)

        self.default_button = tk.Button(button_frame, text="Load Default", command=self.loadDefaultSettings)
        self.default_button.pack(side="right", padx=5)
        
        self.updateUserSettings()  
        
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
        # Simulate loading from a source and update fields
        self.updateUserSettings()

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

    def loadDefaultSettings(self):
        # Load default settings
        self.user_settings.setDefaults()
        self.updateUserSettings()   
