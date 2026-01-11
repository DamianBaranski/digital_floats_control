import tkinter as tk
from tkinter import ttk
from core.protocol import requests
from core.datatypes.monitoring_data import MonitoringData
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, SUCCESS, ERROR, FONT, HEADER_FONT, SECTION_FONT
import time

class MonitoringPanel(tk.Frame):
    def __init__(self, parent, device_client=None):
        tk.Frame.__init__(self, parent, bg=DARK_BG)
        self.device_client = device_client
        self.monitoring_data = {}  # Store monitoring data for each channel
        self.data_ready = [True, True, True, True, True, True]
        self.data_request_time = time.time()
        self.timeout = 60
        self.voltage = None  # Store voltage from StatusData
        self.create_widgets()

    def create_widgets(self):
        # Title
        title = tk.Label(self, text="Monitoring", bg=DARK_BG, fg=TEXT_COLOR, font=HEADER_FONT)
        title.pack(fill="x", pady=(10, 0))
        
        # Create scrollable frame
        canvas = tk.Canvas(self, bg=DARK_BG, highlightthickness=0)
        scrollbar = ttk.Scrollbar(self, orient=tk.VERTICAL, command=canvas.yview)
        scrollable_frame = tk.Frame(canvas, bg=DARK_BG)
        
        scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=10, pady=10)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y, pady=10)
        
        # Channel monitoring data frame
        channels_frame = tk.LabelFrame(scrollable_frame, text="Channel Monitoring Data", 
                                       bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        channels_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Create a grid to display all channels (2 rows x 3 columns)
        self.channel_frames = []
        self.channel_labels = {}  # Store labels for each channel's data
        
        for ch in range(6):
            row = ch // 3
            col = ch % 3
            
            # Channel frame
            channel_frame = tk.Frame(channels_frame, bg=DARK_BG, relief=tk.RAISED, bd=1)
            channel_frame.grid(row=row, column=col, padx=5, pady=5, sticky="nsew")
            
            # Channel header
            channel_header = tk.Label(channel_frame, text=f"Channel {ch}", 
                                     bg=DARKER_BG, fg=TEXT_COLOR, font=(FONT[0], FONT[1], 'bold'))
            channel_header.pack(fill=tk.X, padx=2, pady=2)
            
            # Data section
            data_frame = tk.LabelFrame(channel_frame, text="Status", 
                                      bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
            data_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            # Current
            current_label = tk.Label(data_frame, text="Current: N/A",
                                   bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            current_label.pack(fill=tk.X, padx=5, pady=2)
            self.channel_labels[(ch, 'current')] = current_label
            
            # State
            state_label = tk.Label(data_frame, text="State: N/A",
                                  bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            state_label.pack(fill=tk.X, padx=5, pady=2)
            self.channel_labels[(ch, 'state')] = state_label
            
            # Up switch
            up_switch_label = tk.Label(data_frame, text="Up Switch: N/A",
                                      bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            up_switch_label.pack(fill=tk.X, padx=5, pady=2)
            self.channel_labels[(ch, 'up_switch')] = up_switch_label
            
            # Down switch
            down_switch_label = tk.Label(data_frame, text="Down Switch: N/A",
                                        bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            down_switch_label.pack(fill=tk.X, padx=5, pady=2)
            self.channel_labels[(ch, 'down_switch')] = down_switch_label
            
            self.channel_frames.append(channel_frame)
        
        # Configure grid weights for equal sizing
        channels_frame.grid_columnconfigure(0, weight=1)
        channels_frame.grid_columnconfigure(1, weight=1)
        channels_frame.grid_columnconfigure(2, weight=1)
        channels_frame.grid_rowconfigure(0, weight=1)
        channels_frame.grid_rowconfigure(1, weight=1)
        
        # Voltage frame (shared for all channels)
        voltage_frame = tk.LabelFrame(scrollable_frame, text="System Voltage", 
                                     bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        voltage_frame.pack(fill=tk.X, padx=10, pady=10)
        
        self.voltage_label = tk.Label(voltage_frame, text="Voltage: N/A",
                                     bg=DARK_BG, fg=TEXT_COLOR, font=(FONT[0], FONT[1], 'bold'))
        self.voltage_label.pack(fill=tk.X, padx=10, pady=10)
        
        # Refresh button
        button_frame = tk.Frame(scrollable_frame, bg=DARK_BG)
        button_frame.pack(fill=tk.X, padx=10, pady=10)
        
        refresh_btn = tk.Button(button_frame, text="Refresh", command=self.loadMonitoringData,
                               bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                               activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        refresh_btn.pack(side=tk.LEFT, padx=5)
    
    def update(self):
        if not self.device_client or not self.device_client.isConnected():
            return
            
        if all(self.data_ready) or self.data_request_time + self.timeout < time.time():
            self.data_request_time = time.time()
            self.data_ready = [False] * len(self.data_ready)
            
            # First, request StatusData to get voltage (one request for all channels)
            self.device_client.command(requests.StatusDataRequest(), self._status_data_callback)
            
            # Then request monitoring data for all 6 channels
            for i in range(6):
                self.device_client.command(requests.MonitoringDataRequest(i), 
                                         lambda data, idx=i: self._update_callback(data, idx))

    def _update_callback(self, data, idx):
        if data:
            # Store monitoring data
            self.monitoring_data[idx] = data
            self.data_ready[idx] = True
            # Update display immediately when data is received
            self.updateDisplay()
    
    def _status_data_callback(self, status_data):
        """Callback for StatusData to get voltage (one request for all channels)."""
        if status_data:
            voltage = status_data.get_power_voltage()
            if voltage is not None:
                self.voltage = voltage
                # Update voltage display
                self.voltage_label.config(text=f"Voltage: {voltage:.1f} V", fg=SUCCESS if voltage >= 10.0 else ERROR)
    
    def loadMonitoringData(self):
        """Load monitoring data for all channels."""
        if not self.device_client or not self.device_client.isConnected():
            return
        
        self.data_ready = [False] * 6
        
        # First, request StatusData to get voltage (one request for all channels)
        self.device_client.command(requests.StatusDataRequest(), self._status_data_callback)
        
        # Then request monitoring data for all 6 channels
        for i in range(6):
            self.device_client.command(requests.MonitoringDataRequest(i), 
                                     lambda data, idx=i: self._update_callback(data, idx))
    
    def updateDisplay(self):
        """Update the display with current monitoring data."""
        # Update each channel's display
        for ch in range(6):
            data = self.monitoring_data.get(ch)
            if data:
                # Current
                current_label = self.channel_labels.get((ch, 'current'))
                if current_label:
                    current = data.getCurrent()
                    current_label.config(text=f"Current: {current}", fg=TEXT_COLOR)
                
                # State
                state_label = self.channel_labels.get((ch, 'state'))
                if state_label:
                    state = data.getState()
                    # Color code based on state
                    if state == 'ERROR':
                        state_color = ERROR
                    elif state == 'MOVING':
                        state_color = '#ffaa00'
                    else:
                        state_color = SUCCESS
                    state_label.config(text=f"State: {state}", fg=state_color)
                
                # Up switch
                up_switch_label = self.channel_labels.get((ch, 'up_switch'))
                if up_switch_label:
                    up_switch = data.getUpSwitch()
                    up_color = SUCCESS if up_switch == 'ON' else SECONDARY_TEXT
                    up_switch_label.config(text=f"Up Switch: {up_switch}", fg=up_color)
                
                # Down switch
                down_switch_label = self.channel_labels.get((ch, 'down_switch'))
                if down_switch_label:
                    down_switch = data.getDownSwitch()
                    down_color = SUCCESS if down_switch == 'ON' else SECONDARY_TEXT
                    down_switch_label.config(text=f"Down Switch: {down_switch}", fg=down_color)
            else:
                # No data yet - show N/A
                for key in ['current', 'state', 'up_switch', 'down_switch']:
                    label = self.channel_labels.get((ch, key))
                    if label:
                        label.config(text=f"{key.replace('_', ' ').title()}: N/A", fg=SECONDARY_TEXT)