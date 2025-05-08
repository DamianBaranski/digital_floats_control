import tkinter as tk
from tkinter import ttk
import json
import os
import struct
from typing import List
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, FONT, HEADER_FONT

class UserSettingsPanel:
    def __init__(self, parent):
        self.parent = parent
        self.settings = {}
        self.entries = {}
        self.load_settings()
        self.create_widgets()

    def create_widgets(self):
        # Create main frame
        self.frame = tk.Frame(self.parent, bg=DARK_BG)
        self.frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Create title label
        self.title_label = tk.Label(self.frame, text="User Settings", font=HEADER_FONT,
                                  bg=DARK_BG, fg=TEXT_COLOR)
        self.title_label.pack(pady=(0, 10))

        # Create settings frame
        self.settings_frame = tk.Frame(self.frame, bg=DARK_BG)
        self.settings_frame.pack(fill=tk.BOTH, expand=True)

        # Create settings entries
        for key, value in self.settings.items():
            frame = tk.Frame(self.settings_frame, bg=DARK_BG)
            frame.pack(fill=tk.X, pady=5)

            label = tk.Label(frame, text=key.replace('_', ' ').title(), font=FONT,
                           bg=DARK_BG, fg=TEXT_COLOR)
            label.pack(side=tk.LEFT, padx=(0, 10))

            entry = tk.Entry(frame, font=FONT, bg=DARKER_BG, fg=TEXT_COLOR,
                           insertbackground=TEXT_COLOR)
            entry.insert(0, str(value))
            entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
            self.entries[key] = entry

        # Create buttons frame
        self.buttons_frame = tk.Frame(self.frame, bg=DARK_BG)
        self.buttons_frame.pack(fill=tk.X, pady=(10, 0))

        # Create save button
        self.save_button = tk.Button(self.buttons_frame, text="Save", command=self.save_settings,
                                   bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                   activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.save_button.pack(side=tk.RIGHT, padx=5)

        # Create reset button
        self.reset_button = tk.Button(self.buttons_frame, text="Reset", command=self.reset_settings,
                                    bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                                    activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        self.reset_button.pack(side=tk.RIGHT, padx=5)

    def load_settings(self):
        try:
            with open('user_settings.json', 'r') as f:
                self.settings = json.load(f)
        except FileNotFoundError:
            # Set default settings
            self.settings = {
                'default_port': 'COM1',
                'baud_rate': '9600',
                'timeout': '1.0',
                'retry_count': '3',
                'brightness': '128',
                'ldg_up_color': '#0000FF',
                'ldg_down_color': '#00FF00',
                'rudder_up_color': '#00FF00',
                'rudder_down_color': '#0000FF',
                'rudder_inactive_color': '#FFFF00',
                'warning_color': '#FFA500',
                'error_color': '#FF0000'
            }
            # Save default settings to file
            try:
                with open('user_settings.json', 'w') as f:
                    json.dump(self.settings, f, indent=4)
            except Exception as e:
                print(f"Warning: Could not save default settings: {e}")

    def save_settings(self):
        # Update settings from entries
        for key, entry in self.entries.items():
            self.settings[key] = entry.get()
        
        # Save to file
        try:
            with open('user_settings.json', 'w') as f:
                json.dump(self.settings, f, indent=4)
        except Exception as e:
            print(f"Error saving settings: {e}")

    def reset_settings(self):
        self.load_settings()
        for key, entry in self.entries.items():
            entry.delete(0, tk.END)
            entry.insert(0, str(self.settings[key]))

    def get_setting(self, key, default=None):
        return self.settings.get(key, default)

