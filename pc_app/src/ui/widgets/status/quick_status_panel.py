from ui.theme import DARK_BG, DARKER_BG, BORDER_COLOR, TEXT_COLOR, SECONDARY_TEXT, SUCCESS, ERROR, FONT, HEADER_FONT, SECTION_FONT, ACCENT
from ui.widgets.base.tooltip import Tooltip

import tkinter as tk
from tkinter import ttk
import datetime

class QuickStatusPanel(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent, bg=DARK_BG)
        self.configure(bg=DARK_BG)
        self.create_widgets()

    def create_widgets(self):
        # Title at the top
        title = tk.Label(self, text="CONNECTED DEVICE INFORMATION", bg=DARK_BG, fg=TEXT_COLOR, font=HEADER_FONT)
        title.pack(fill="x", pady=(10, 0))
        Tooltip(title, "This section displays information about the currently connected device.")

        # Section: System Information
        sysinfo_frame = self.section_frame("System Information", "Displays basic system information about the connected device including firmware version, hardware details, and manufacturing information.")
        self.fw_version = self.info_row(sysinfo_frame, "Firmware Version:", "N/A", col=2)
        self.hw_version = self.info_row(sysinfo_frame, "Hardware Version:", "N/A", col=2)
        self.hw_config = self.info_row(sysinfo_frame, "Hardware Configuration:", "N/A", col=2)
        self.mfg_date = self.info_row(sysinfo_frame, "Mfg Date:", "N/A", col=2)
        self.last_update = self.info_row(sysinfo_frame, "Last FW Update:", "N/A", col=2)
        sysinfo_frame.pack(fill="x", padx=10, pady=(10, 0))

        # Section: System Health
        health_frame = self.section_frame("System Health", "Shows the current health status of the system including uptime, power status, memory usage, and communication load.")
        self.uptime = self.info_row(health_frame, "System Uptime:", "N/A", col=2, pady=6)
        self.power_status = self.info_row(health_frame, "Power Status:", "N/A", fg=SUCCESS, col=2, pady=6)
        self.health_label = self.health_text_row(health_frame, "Overall Health:", 98, "Good", color=SUCCESS, col=2, pady=6)
        self.mem_label, self.mem_bar = self.health_row_3col(health_frame, "Memory Usage:", 45, "180/400 MB (45%)", color=SUCCESS, pady=6)
        self.uart_label, self.uart_bar = self.health_row_3col(health_frame, "UART Load:", 12, "1.2 kB/s (12%)", color=SUCCESS, pady=6)
        health_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Operation Status
        op_frame = self.section_frame("Operation Status", "Displays the current operational status of the device including remote control mode and test results.")
        self.remote_mode = self.info_row(op_frame, "Remote Control Mode:", "OFF", fg=ERROR, col=2, pady=6)
        self.last_test = self.info_row(op_frame, "Last Test Status:", "Success (2025-05-07 12:10)", fg=SUCCESS, col=2, pady=6)
        self.last_comm = self.info_row(op_frame, "Last Communication:", "2025-05-07 12:12", col=2, pady=6)
        op_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Controls
        ctrl_frame = self.section_frame("Controls", "Provides control buttons for system operations including reset, remote control mode, and firmware updates.")
        self.reset_btn = tk.Button(ctrl_frame, text="System Reset", bg=DARKER_BG, fg=TEXT_COLOR, activebackground=SUCCESS, relief=tk.RAISED, font=FONT, borderwidth=1, highlightthickness=0, command=self.mock_reset)
        self.reset_btn.grid(row=0, column=0, sticky="ew", pady=4, columnspan=3)
        self.remote_btn = tk.Button(ctrl_frame, text="Enable Remote Control Mode", bg=DARKER_BG, fg=TEXT_COLOR, activebackground=ACCENT, relief=tk.RAISED, font=FONT, borderwidth=1, highlightthickness=0, command=self.toggle_remote_mode)
        self.remote_btn.grid(row=1, column=0, sticky="ew", pady=4, columnspan=3)
        self.fw_update_btn = tk.Button(ctrl_frame, text="Firmware Update", bg=DARKER_BG, fg=TEXT_COLOR, activebackground=ACCENT, relief=tk.RAISED, font=FONT, borderwidth=1, highlightthickness=0, command=self.open_fw_update_popup)
        self.fw_update_btn.grid(row=2, column=0, sticky="ew", pady=4, columnspan=3)
        ctrl_frame.pack(fill="x", padx=10, pady=(15, 10))

        # Make responsive
        for frame in [sysinfo_frame, health_frame, op_frame, ctrl_frame]:
            frame.columnconfigure(0, weight=1)
            frame.columnconfigure(1, weight=1)
            frame.columnconfigure(2, weight=2)
        self.pack_propagate(False)

        # State for remote control mode
        self.remote_enabled = False

    def section_frame(self, title, tooltip_text=None):
        frame = tk.Frame(self, bg=DARK_BG)
        divider = tk.Frame(self, bg=BORDER_COLOR, height=2)
        divider.pack(fill="x", padx=5, pady=(10, 0))
        label = tk.Label(frame, text=title, bg=DARK_BG, fg=TEXT_COLOR, font=SECTION_FONT)
        label.grid(row=0, column=0, columnspan=3, sticky="w", pady=(0, 5))
        if tooltip_text:
            Tooltip(frame, tooltip_text)
        return frame

    def info_row(self, parent, label, value, fg=TEXT_COLOR, col=2, pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT).grid(row=row, column=0, sticky="w", pady=pady)
        val = tk.Label(parent, text=value, bg=DARK_BG, fg=fg, font=("Segoe UI", 10, "bold"))
        val.grid(row=row, column=col, sticky="e", pady=pady)
        return val

    def health_text_row(self, parent, label, percent, text, color=SUCCESS, col=2, pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT).grid(row=row, column=0, sticky="w", pady=pady)
        label2 = tk.Label(parent, text=f"{text} ({percent}%)", bg=DARK_BG, fg=color, font=("Segoe UI", 10, "bold"))
        label2.grid(row=row, column=col, sticky="e", padx=5, pady=pady)
        return label2

    def health_row_3col(self, parent, label, percent, text, color=SUCCESS, pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg=DARK_BG, fg=SECONDARY_TEXT, font=FONT).grid(row=row, column=0, sticky="w", pady=pady)
        bar = ttk.Progressbar(parent, value=percent, maximum=100, length=90)
        bar.grid(row=row, column=1, sticky="", padx=(0, 5), pady=pady)
        bar_style = ttk.Style()
        bar_style.theme_use('default')
        bar_style.configure(f"{label}_bar.Horizontal.TProgressbar", troughcolor=DARKER_BG, background=color, bordercolor=DARK_BG, lightcolor=color, darkcolor=color)
        bar.configure(style=f"{label}_bar.Horizontal.TProgressbar")
        label2 = tk.Label(parent, text=text, bg=DARK_BG, fg=color, font=("Segoe UI", 10, "bold"))
        label2.grid(row=row, column=2, sticky="e", padx=5, pady=pady)
        return label2, bar

    def mock_update(self):
        # This would be replaced with real data updates
        now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.uptime.config(text="00:13:37")
        self.last_comm.config(text=now)
        self.after(1000, self.mock_update)

    def mock_reset(self):
        self.last_test.config(text="Reset ({} Success)".format(datetime.datetime.now().strftime("%H:%M:%S")), fg=SUCCESS)

    def toggle_remote_mode(self):
        self.remote_enabled = not self.remote_enabled
        if self.remote_enabled:
            self.remote_mode.config(text="ON", fg=SUCCESS)
            self.remote_btn.config(text="Disable Remote Control Mode", bg=ACCENT)
        else:
            self.remote_mode.config(text="OFF", fg=ERROR)
            self.remote_btn.config(text="Enable Remote Control Mode", bg=DARKER_BG)

    def open_fw_update_popup(self):
        popup = tk.Toplevel(self)
        popup.title("Firmware Update")
        popup.configure(bg=DARK_BG)
        tk.Label(popup, text="Release:", bg=DARK_BG, fg=TEXT_COLOR).pack(padx=10, pady=(10, 2), anchor="w")
        release_var = tk.StringVar(value="v1.2.3")
        option_var = tk.StringVar(value="Newest")
        option_menu = tk.OptionMenu(popup, option_var, "Newest", "Other")
        option_menu.config(bg=DARKER_BG, fg=TEXT_COLOR, highlightthickness=0, activebackground=ACCENT)
        option_menu.pack(padx=10, pady=2, fill="x")
        entry = tk.Entry(popup, textvariable=release_var, bg=DARKER_BG, fg=SECONDARY_TEXT, insertbackground=TEXT_COLOR, state="disabled")
        entry.pack(padx=10, pady=2, fill="x")
        btn_frame = tk.Frame(popup, bg=DARK_BG)
        btn_frame.pack(padx=10, pady=10, fill="x")
        def on_option_change(*args):
            if option_var.get() == "Other":
                entry.config(state="normal", fg=TEXT_COLOR)
            else:
                entry.config(state="disabled", fg=SECONDARY_TEXT)
        option_var.trace_add("write", on_option_change)
        def do_update():
            # Here you would trigger the real update
            self.last_update.config(text=datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))
            popup.destroy()
        tk.Button(btn_frame, text="Update", command=do_update, bg=SUCCESS, fg=TEXT_COLOR).pack(side="left", expand=True, fill="x", padx=(0,5))
        tk.Button(btn_frame, text="Cancel", command=popup.destroy, bg=DARKER_BG, fg=TEXT_COLOR).pack(side="left", expand=True, fill="x", padx=(5,0))
        entry.focus_set()
        
    def update_uptime(self, uptime):
        minutes, secs = divmod(int(uptime), 60)
        hours, minutes = divmod(minutes, 60)
        self.uptime.config(text=f"{hours:02d}:{minutes:02d}:{secs:02d}")