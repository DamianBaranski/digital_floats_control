from core.protocol.device_client import DeviceClient
from core.protocol import requests
from ui.widgets.base.serial_port_panel import SerialPortPanel
from ui.widgets.status.system_status_panel import SystemStatusPanel
from ui.widgets.settings.app_settings_panel import AppSettingsPanel
from ui.widgets.monitoring.monitoring_panel import MonitoringPanel
from ui.widgets.error_status.channel_error_status_panel import ChannelErrorStatusPanel
from ui.widgets.logs.log_output_panel import LogOutputPanel
from ui.widgets.base.detachable_notebook import DetachableNotebook
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, FONT, HEADER_FONT

import tkinter as tk
from tkinter import ttk

class DigitalFloatsApp(tk.Frame):
    def __init__(self, parent):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.parent = parent
        self.parent.title("Digital Floats App")
        self.parent.configure(bg=DARK_BG)
        self.device_client = DeviceClient()
        self.device_client.connect("/dev/ttyUSB0")
        # Status data will be loaded on connection via SystemStatusPanel.update()
        # Firmware info will be loaded on connection via QuickStatusPanel.loadFirmwareInfo()

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

        # Create a frame for the left column (including ComPortWidget and other widgets, if needed)
        self.left_frame = tk.Frame(self, bg=DARK_BG)
        self.left_frame.grid(row=0, column=0, sticky="ns")

        # Place the ComPortWidget at the top of the left column
        self.ui_port = SerialPortPanel(self.left_frame, self.connect_to_device)
        self.ui_port.pack(padx=10, pady=10, side=tk.TOP, fill=tk.X)

        # Add a vertical separator between left and right columns
        self.vert_separator = ttk.Separator(self, orient='vertical')
        self.vert_separator.grid(row=0, column=1, sticky='ns')

        # Tab factories for robust detach/reattach
        tab_factories = {
            "Status": lambda parent: SystemStatusPanel(parent, self.device_client),
            "Settings": lambda parent: AppSettingsPanel(parent, self.device_client),
            "Monitoring": lambda parent: MonitoringPanel(parent, self.device_client),
            "Channel Error Status": lambda parent: ChannelErrorStatusPanel(parent, self.device_client),
            "Logs": lambda parent: LogOutputPanel(parent),
        }

        # Create the detachable notebook for tabs
        self.ui_tabs = DetachableNotebook(self, tab_factories=tab_factories)
        self.ui_tabs.grid(row=0, column=2, sticky="nsew")  # Fill horizontally and vertically

        self.tabs = {}
        # Create and add tabs to the notebook using factories
        for tab_name in ["Status", "Settings", "Monitoring", "Channel Error Status", "Logs"]:
            frame = tk.Frame(self.ui_tabs, bg=DARK_BG)
            widget = tab_factories[tab_name](frame)
            widget.pack(fill='both', expand=True)
            self.ui_tabs.add(frame, text=tab_name)
            self.tabs[tab_name] = widget

        # Create and place the status bar at the bottom
        self.ui_status_bar = ttk.Label(parent, relief=tk.SUNKEN, anchor="w", background=DARKER_BG, foreground=TEXT_COLOR, font=FONT)
        self.ui_status_bar.grid(row=1, column=0, columnspan=3, sticky="ew")

        # Configure the grid for resizing behavior
        self.grid_rowconfigure(0, weight=1)  # Make row 0 expandable in DigitalFloatsApp
        self.grid_columnconfigure(2, weight=1)  # Make column 2 expandable in DigitalFloatsApp
        
        # Bind the window close event
        parent.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.ui_port.setPorts(self.device_client.getPortList())

    def connect_to_device(self, port):
        if self.device_client.isConnected():
            self.device_client.disconnect()
        else:
            self.device_client.connect(port)
            # Load firmware info and status data when connected
            if self.device_client.isConnected():
                if 'Status' in self.tabs:
                    if hasattr(self.tabs['Status'], 'quick_status'):
                        self.tabs['Status'].quick_status.loadFirmwareInfo()
                        self.tabs['Status'].quick_status.loadStatusData()
                # Load monitoring data when connected
                if 'Monitoring' in self.tabs:
                    self.tabs['Monitoring'].loadMonitoringData()
                # Load error status when connected
                if 'Channel Error Status' in self.tabs:
                    self.tabs['Channel Error Status'].loadErrorStatus()
        self.ui_port.setStatus(self.device_client.isConnected())
        
    def on_closing(self):
        self.parent.destroy()   # Destroy the window

    def on_connected(self, status):
        if status:
            for frame in self.ui_tabs.winfo_children():
                if self.ui_tabs.tab(frame, option='text') == "Status":
                    for child in frame.winfo_children():
                        if hasattr(child, 'update'):
                            child.update()
                            
    def firmware_version_update(self, version: str):
        print(version)
    
    def status_update(self, status):
        self.tabs['Status'].setStatus(status)
