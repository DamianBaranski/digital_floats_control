import tkinter as tk
from tkinter import ttk, colorchooser
import json
from ui.widgets.settings.user_settings_panel import UserSettingsPanel
from src.core.datatypes.channel_settings import ChannelSettings
from ui.widgets.settings.channel_settings_table_panel import ChannelSettingsTablePanel
from ui.widgets.settings.auto_detect import AutoDetect
from core.protocol import requests
from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, SUCCESS, FONT, HEADER_FONT, SECTION_FONT

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
    def __init__(self, parent, device_client=None):
        super().__init__(parent, bg=DARK_BG)
        self.device_client = device_client
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
        channel_frame = tk.LabelFrame(self.scrollable_frame, text="Channel Settings", 
                                     bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        channel_frame.pack(fill="both", expand=True, padx=10, pady=10)

        # Channel Settings Buttons (at top) - use separate container to avoid pack/grid conflict
        button_container = tk.Frame(channel_frame, bg=DARK_BG)
        button_container.pack(fill="x", padx=10, pady=10)
        
        button_frame = tk.Frame(button_container, bg=DARK_BG)
        button_frame.pack(side="right")

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

        # Channels grid container (separate frame for grid to avoid pack/grid conflict)
        channels_grid_frame = tk.Frame(channel_frame, bg=DARK_BG)
        channels_grid_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Create grid to display all channels (2 rows x 3 columns)
        self.channel_frames = []
        self.channel_labels = {}  # Store labels for each channel's settings
        
        # Initialize channel settings table (still needed for data storage and EditDialog)
        self.channel_settings_table = ChannelSettingsTablePanel(channel_frame)
        self.channel_settings_table.pack_forget()  # Hide the table, but keep it for data management
        
        for ch in range(6):
            row = ch // 3
            col = ch % 3
            
            # Channel frame
            ch_frame = tk.Frame(channels_grid_frame, bg=DARK_BG, relief=tk.RAISED, bd=1)
            ch_frame.grid(row=row, column=col, padx=5, pady=5, sticky="nsew")
            
            # Channel header with edit button
            header_frame = tk.Frame(ch_frame, bg=DARKER_BG)
            header_frame.pack(fill=tk.X, padx=2, pady=2)
            
            channel_header = tk.Label(header_frame, text=f"Channel {ch}", 
                                     bg=DARKER_BG, fg=TEXT_COLOR, font=(FONT[0], FONT[1], 'bold'))
            channel_header.pack(side=tk.LEFT, padx=5)
            
            edit_btn = tk.Button(header_frame, text="Edit", 
                              command=lambda idx=ch: self._edit_channel(idx),
                              bg=BORDER_COLOR, fg=TEXT_COLOR, font=("TkDefaultFont", 8),
                              activebackground=TEXT_COLOR, activeforeground=DARK_BG)
            edit_btn.pack(side=tk.RIGHT, padx=2)
            
            # Settings display section
            settings_frame = tk.LabelFrame(ch_frame, text="Settings", 
                                          bg=DARK_BG, fg=TEXT_COLOR, font=FONT)
            settings_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
            
            # Enable
            enable_label = tk.Label(settings_frame, text="Enable: N/A",
                                  bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            enable_label.pack(fill=tk.X, padx=5, pady=1)
            self.channel_labels[(ch, 'enable')] = enable_label
            
            # Bridge
            bridge_label = tk.Label(settings_frame, text="Bridge: N/A",
                                  bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            bridge_label.pack(fill=tk.X, padx=5, pady=1)
            self.channel_labels[(ch, 'bridge')] = bridge_label
            
            # Rudder
            rudder_label = tk.Label(settings_frame, text="Rudder: N/A",
                                  bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            rudder_label.pack(fill=tk.X, padx=5, pady=1)
            self.channel_labels[(ch, 'rudder')] = rudder_label
            
            # Timeout
            timeout_label = tk.Label(settings_frame, text="Timeout: N/A",
                                    bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            timeout_label.pack(fill=tk.X, padx=5, pady=1)
            self.channel_labels[(ch, 'timeout')] = timeout_label
            
            # Current limits (summary)
            current_label = tk.Label(settings_frame, text="Current: N/A",
                                   bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT, anchor='w')
            current_label.pack(fill=tk.X, padx=5, pady=1)
            self.channel_labels[(ch, 'current')] = current_label
            
            self.channel_frames.append(ch_frame)
        
        # Configure grid weights for equal sizing
        channels_grid_frame.grid_columnconfigure(0, weight=1)
        channels_grid_frame.grid_columnconfigure(1, weight=1)
        channels_grid_frame.grid_columnconfigure(2, weight=1)
        channels_grid_frame.grid_rowconfigure(0, weight=1)
        channels_grid_frame.grid_rowconfigure(1, weight=1)
    
    def _edit_channel(self, channel_idx):
        """Open edit dialog for a specific channel."""
        # Ensure data exists for this channel
        self.channel_settings_table.addData(channel_idx + 1)
        
        # Get or create settings for this channel
        try:
            settings = self.channel_settings_table.getData(channel_idx)
        except:
            settings = ChannelSettings()
            settings.setChannel(channel_idx)
            self.channel_settings_table.setData(channel_idx, settings)
        
        if not settings:
            settings = ChannelSettings()
            settings.setChannel(channel_idx)
            self.channel_settings_table.setData(channel_idx, settings)
        
        # Open edit dialog (EditDialog is in channel_settings_table_panel)
        from ui.widgets.settings.channel_settings_table_panel import EditDialog
        EditDialog(self, settings, lambda s: self._on_channel_updated(channel_idx, s))
    
    def _on_channel_updated(self, channel_idx, settings):
        """Callback when channel settings are updated via edit dialog."""
        self.channel_settings_table.setData(channel_idx, settings)
        self.updateChannelDisplay(channel_idx)
    
    def updateChannelDisplay(self, channel_idx):
        """Update the display for a specific channel."""
        settings = self.channel_settings_table.getData(channel_idx)
        if settings:
            # Enable
            enable_label = self.channel_labels.get((channel_idx, 'enable'))
            if enable_label:
                enabled = settings.getEnable()
                enable_label.config(text=f"Enable: {'✓' if enabled else '✗'}", 
                                  fg=SUCCESS if enabled else SECONDARY_TEXT)
            
            # Bridge
            bridge_label = self.channel_labels.get((channel_idx, 'bridge'))
            if bridge_label:
                bridge = settings.getBridge()
                bridge_label.config(text=f"Bridge: {'✓' if bridge else '✗'}", 
                                  fg=SUCCESS if bridge else SECONDARY_TEXT)
            
            # Rudder
            rudder_label = self.channel_labels.get((channel_idx, 'rudder'))
            if rudder_label:
                rudder = settings.getRudder()
                rudder_label.config(text=f"Rudder: {'✓' if rudder else '✗'}", 
                                  fg=SUCCESS if rudder else SECONDARY_TEXT)
            
            # Timeout
            timeout_label = self.channel_labels.get((channel_idx, 'timeout'))
            if timeout_label:
                timeout = settings.getTimeout()
                timeout_label.config(text=f"Timeout: {timeout}s", fg=TEXT_COLOR)
            
            # Current limits (summary)
            current_label = self.channel_labels.get((channel_idx, 'current'))
            if current_label:
                warn = settings.get('max_current_warning_limit')
                err = settings.get('max_current_error_limit')
                min_curr = settings.get('min_current_limit')
                current_label.config(text=f"Current: W:{warn:.2f}A E:{err:.2f}A", 
                                   fg=TEXT_COLOR)

    def autoDetect(self):
        pass
        #auto_detect = AutoDetect(self.app_protocol, self.channel_settings_table)
        #auto_detect.start()

    def channelHelp(self):
        self.channel_settings_table.display_instructions()

    def loadChannelSettings(self):
        if not self.device_client:
            return
            
        status = [False] * 6
        def callback(channel_settings, idx):
            status[idx] = True
            self.channel_settings_table.addData(idx + 1)
            self.channel_settings_table.setData(idx, channel_settings)
            self.updateChannelDisplay(idx)
            if all(status):
                print("All channel settings loaded")

        for i in range(6):
            self.device_client.command(requests.GetChannelSettingsRequest(i), lambda data, idx=i: callback(data, idx))

    def saveChannelSettings(self):
        if not self.device_client:
            return
            
        for i in range(6):
            channel_settings = self.channel_settings_table.getData(i)
            if channel_settings:
                self.device_client.command(requests.UpdateChannelSettingsRequest(channel_settings), lambda success: print(f"Channel {i} settings saved: {success}"))
            