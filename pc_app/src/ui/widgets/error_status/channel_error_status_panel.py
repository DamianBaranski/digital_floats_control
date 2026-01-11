import tkinter as tk
from tkinter import ttk
from core.protocol import requests
from core.datatypes.error_status import ChannelError, ChannelWarning, SystemWarning
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, SUCCESS, ERROR, FONT, HEADER_FONT, SECTION_FONT

class ChannelErrorStatusPanel(tk.Frame):
    def __init__(self, parent, device_client=None):
        super().__init__(parent, bg=DARK_BG)
        self.device_client = device_client
        self.error_status = None
        self.create_widgets()
        
    def create_widgets(self):
        # Title
        title = tk.Label(self, text="Channel Error Status", bg=DARK_BG, fg=TEXT_COLOR, font=HEADER_FONT)
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
        
        # Channel errors and warnings frame
        channels_frame = tk.LabelFrame(scrollable_frame, text="Channel Errors & Warnings", 
                                       bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        channels_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Create a grid to display all channels (2 rows x 3 columns)
        self.channel_frames = []
        self.error_labels = {}
        self.warning_labels = {}
        
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
            
            # Errors section
            errors_frame = tk.LabelFrame(channel_frame, text="Errors", 
                                        bg=DARK_BG, fg=ERROR, font=FONT)
            errors_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            for error in ChannelError:
                label = tk.Label(errors_frame, text=error.name.replace('_', ' ').title(),
                               bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
                label.pack(fill=tk.X, padx=5, pady=1)
                self.error_labels[(ch, error)] = label
            
            # Warnings section
            warnings_frame = tk.LabelFrame(channel_frame, text="Warnings", 
                                          bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
            warnings_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            for warning in ChannelWarning:
                label = tk.Label(warnings_frame, text=warning.name.replace('_', ' ').title(),
                               bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
                label.pack(fill=tk.X, padx=5, pady=1)
                self.warning_labels[(ch, warning)] = label
            
            self.channel_frames.append(channel_frame)
        
        # Configure grid weights for equal sizing
        channels_frame.grid_columnconfigure(0, weight=1)
        channels_frame.grid_columnconfigure(1, weight=1)
        channels_frame.grid_columnconfigure(2, weight=1)
        channels_frame.grid_rowconfigure(0, weight=1)
        channels_frame.grid_rowconfigure(1, weight=1)
        
        # System warnings frame
        system_frame = tk.LabelFrame(scrollable_frame, text="System Warnings", 
                                    bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        system_frame.pack(fill=tk.X, padx=10, pady=10)
        
        self.system_warning_labels = {}
        for warning in SystemWarning:
            label = tk.Label(system_frame, text=warning.name.replace('_', ' ').title(),
                           bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            label.pack(fill=tk.X, padx=10, pady=5)
            self.system_warning_labels[warning] = label
        
        # Refresh button
        button_frame = tk.Frame(scrollable_frame, bg=DARK_BG)
        button_frame.pack(fill=tk.X, padx=10, pady=10)
        
        refresh_btn = tk.Button(button_frame, text="Refresh", command=self.loadErrorStatus,
                               bg=DARKER_BG, fg=TEXT_COLOR, font=FONT,
                               activebackground=BORDER_COLOR, activeforeground=TEXT_COLOR)
        refresh_btn.pack(side=tk.LEFT, padx=5)
        
    def loadErrorStatus(self):
        """Load error status from device."""
        if not self.device_client or not self.device_client.isConnected():
            return
        
        def callback(error_status):
            if error_status:
                self.error_status = error_status
                self.updateDisplay()
                print("Error status loaded:", error_status)
        
        self.device_client.command(requests.ErrorStatusRequest(), callback)
    
    def updateDisplay(self):
        """Update the display with current error status."""
        if not self.error_status:
            return
        
        # Update channel errors
        for ch in range(6):
            for error in ChannelError:
                label = self.error_labels.get((ch, error))
                if label:
                    is_set = self.error_status.isSet(ch, error)
                    if is_set:
                        label.config(text=f"✓ {error.name.replace('_', ' ').title()}", 
                                   fg=ERROR, font=(FONT[0], FONT[1], 'bold'))
                    else:
                        label.config(text=f"  {error.name.replace('_', ' ').title()}", 
                                   fg=SECONDARY_TEXT, font=FONT)
        
        # Update channel warnings
        for ch in range(6):
            for warning in ChannelWarning:
                label = self.warning_labels.get((ch, warning))
                if label:
                    is_set = self.error_status.isSet(ch, warning)
                    if is_set:
                        label.config(text=f"⚠ {warning.name.replace('_', ' ').title()}", 
                                   fg='#ffaa00', font=(FONT[0], FONT[1], 'bold'))
                    else:
                        label.config(text=f"  {warning.name.replace('_', ' ').title()}", 
                                   fg=SECONDARY_TEXT, font=FONT)
        
        # Update system warnings
        for warning in SystemWarning:
            label = self.system_warning_labels.get(warning)
            if label:
                is_set = self.error_status.isSet(0, warning)  # Channel parameter ignored for SystemWarning
                if is_set:
                    label.config(text=f"⚠ {warning.name.replace('_', ' ').title()}", 
                               fg='#ffaa00', font=(FONT[0], FONT[1], 'bold'))
                else:
                    label.config(text=f"  {warning.name.replace('_', ' ').title()}", 
                               fg=SECONDARY_TEXT, font=FONT)

