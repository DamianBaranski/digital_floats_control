import tkinter as tk
from tkinter import ttk
import datetime

class QuickStatusWidget(tk.Frame):
    def __init__(self, parent):
        super().__init__(parent, bg="#23272a")
        self.configure(bg="#23272a")
        self.create_widgets()
        self.mock_update()

    def create_widgets(self):
        # Section: System Information
        sysinfo_frame = self.section_frame("System Information")
        self.fw_version = self.info_row(sysinfo_frame, "Firmware Version:", "v1.2.3")
        self.last_update = self.info_row(sysinfo_frame, "Last FW Update:", "2025-05-07 12:00")
        self.uptime = self.info_row(sysinfo_frame, "System Uptime:", "00:12:34")
        self.power_status = self.info_row(sysinfo_frame, "Power Status:", "13.2 V", fg="#43d17a")
        sysinfo_frame.pack(fill="x", padx=10, pady=(10, 0))

        # Section: System Health
        health_frame = self.section_frame("System Health")
        self.health_label, self.health_bar = self.health_row(health_frame, "Overall Health:", 98, "Good", color="#43d17a")
        self.mem_label, self.mem_bar = self.health_row(health_frame, "Memory Usage:", 45, "180/400 MB", color="#43d17a")
        self.uart_label, self.uart_bar = self.health_row(health_frame, "UART Load:", 12, "12% (1.2 kB/s)", color="#43d17a")
        health_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Operation Status
        op_frame = self.section_frame("Operation Status")
        self.remote_mode = self.status_row(op_frame, "Remote Control Mode:", "OFF", fg="#e74c3c")
        self.last_test = self.status_row(op_frame, "Last Test Status:", "Success (2025-05-07 12:10)", fg="#43d17a")
        self.last_comm = self.status_row(op_frame, "Last Communication:", "2025-05-07 12:12")
        op_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Controls
        ctrl_frame = self.section_frame("Controls")
        self.reset_btn = tk.Button(ctrl_frame, text="System Reset", bg="#2d333b", fg="#fff", activebackground="#43d17a", relief=tk.RAISED, command=self.mock_reset)
        self.reset_btn.grid(row=0, column=0, sticky="ew", pady=4, columnspan=2)
        self.remote_btn = tk.Button(ctrl_frame, text="Enable Remote Control Mode", bg="#2d333b", fg="#fff", activebackground="#7289da", relief=tk.RAISED, command=self.toggle_remote_mode)
        self.remote_btn.grid(row=1, column=0, sticky="ew", pady=4, columnspan=2)
        ctrl_frame.pack(fill="x", padx=10, pady=(15, 10))

        # Make responsive
        for frame in [sysinfo_frame, health_frame, op_frame, ctrl_frame]:
            frame.columnconfigure(1, weight=1)
        self.pack_propagate(False)

        # State for remote control mode
        self.remote_enabled = False

    def section_frame(self, title):
        frame = tk.Frame(self, bg="#23272a")
        divider = tk.Frame(self, bg="#36393f", height=2)
        divider.pack(fill="x", padx=5, pady=(10, 0))
        label = tk.Label(frame, text=title, bg="#23272a", fg="#fff", font=("Segoe UI", 11, "bold"))
        label.grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 5))
        return frame

    def info_row(self, parent, label, value, fg="#fff"):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w")
        val = tk.Label(parent, text=value, bg="#23272a", fg=fg, font=("Segoe UI", 10, "bold"))
        val.grid(row=row, column=1, sticky="e")
        return val

    def health_row(self, parent, label, percent, text, color="#43d17a"):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w")
        bar = ttk.Progressbar(parent, value=percent, maximum=100, length=120)
        bar.grid(row=row, column=1, sticky="ew", padx=5)
        bar_style = ttk.Style()
        bar_style.theme_use('default')
        bar_style.configure(f"{label}_bar.Horizontal.TProgressbar", troughcolor="#2d333b", background=color, bordercolor="#23272a", lightcolor=color, darkcolor=color)
        bar.configure(style=f"{label}_bar.Horizontal.TProgressbar")
        label2 = tk.Label(parent, text=f"{text} ({percent}%)" if isinstance(text, str) and not text.endswith('%') else text, bg="#23272a", fg=color, font=("Segoe UI", 10, "bold"))
        label2.grid(row=row, column=2, sticky="e", padx=5)
        return label2, bar

    def status_row(self, parent, label, value, fg="#fff"):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w")
        val = tk.Label(parent, text=value, bg="#23272a", fg=fg, font=("Segoe UI", 10, "bold"))
        val.grid(row=row, column=1, sticky="e")
        return val

    def mock_update(self):
        # This would be replaced with real data updates
        now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.uptime.config(text="00:13:37")
        self.last_comm.config(text=now)
        self.after(1000, self.mock_update)

    def mock_reset(self):
        self.last_test.config(text="Reset ({} Success)".format(datetime.datetime.now().strftime("%H:%M:%S")), fg="#43d17a")

    def toggle_remote_mode(self):
        self.remote_enabled = not self.remote_enabled
        if self.remote_enabled:
            self.remote_mode.config(text="ON", fg="#43d17a")
            self.remote_btn.config(text="Disable Remote Control Mode", bg="#7289da")
        else:
            self.remote_mode.config(text="OFF", fg="#e74c3c")
            self.remote_btn.config(text="Enable Remote Control Mode", bg="#2d333b") 