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
        # Title at the top
        title = tk.Label(self, text="CONNECTED DEVICE INFORMATION", bg="#23272a", fg="#fff", font=("Segoe UI", 14, "bold"))
        title.pack(fill="x", pady=(10, 0))

        # Section: System Information
        sysinfo_frame = self.section_frame("System Information")
        self.fw_version = self.info_row(sysinfo_frame, "Firmware Version:", "v1.2.3", col=2)
        self.hw_version = self.info_row(sysinfo_frame, "Hardware Version:", "HW-2024A", col=2)
        self.hw_config = self.info_row(sysinfo_frame, "Hardware Configuration:", "STD-4CH", col=2)
        self.mfg_date = self.info_row(sysinfo_frame, "Mfg Date:", "2024-05-01", col=2)
        self.last_update = self.info_row(sysinfo_frame, "Last FW Update:", "2025-05-07 12:00", col=2)
        sysinfo_frame.pack(fill="x", padx=10, pady=(10, 0))

        # Section: System Health
        health_frame = self.section_frame("System Health")
        self.uptime = self.info_row(health_frame, "System Uptime:", "00:12:34", col=2, pady=6)
        self.power_status = self.info_row(health_frame, "Power Status:", "13.2 V", fg="#43d17a", col=2, pady=6)
        self.health_label = self.health_text_row(health_frame, "Overall Health:", 98, "Good", color="#43d17a", col=2, pady=6)
        self.mem_label, self.mem_bar = self.health_row_3col(health_frame, "Memory Usage:", 45, "180/400 MB (45%)", color="#43d17a", pady=6)
        self.uart_label, self.uart_bar = self.health_row_3col(health_frame, "UART Load:", 12, "1.2 kB/s (12%)", color="#43d17a", pady=6)
        health_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Operation Status
        op_frame = self.section_frame("Operation Status")
        self.remote_mode = self.info_row(op_frame, "Remote Control Mode:", "OFF", fg="#e74c3c", col=2, pady=6)
        self.last_test = self.info_row(op_frame, "Last Test Status:", "Success (2025-05-07 12:10)", fg="#43d17a", col=2, pady=6)
        self.last_comm = self.info_row(op_frame, "Last Communication:", "2025-05-07 12:12", col=2, pady=6)
        op_frame.pack(fill="x", padx=10, pady=(15, 0))

        # Section: Controls
        ctrl_frame = self.section_frame("Controls")
        self.reset_btn = tk.Button(ctrl_frame, text="System Reset", bg="#2d333b", fg="#fff", activebackground="#43d17a", relief=tk.RAISED, command=self.mock_reset)
        self.reset_btn.grid(row=0, column=0, sticky="ew", pady=4, columnspan=3)
        self.remote_btn = tk.Button(ctrl_frame, text="Enable Remote Control Mode", bg="#2d333b", fg="#fff", activebackground="#7289da", relief=tk.RAISED, command=self.toggle_remote_mode)
        self.remote_btn.grid(row=1, column=0, sticky="ew", pady=4, columnspan=3)
        self.fw_update_btn = tk.Button(ctrl_frame, text="Firmware Update", bg="#2d333b", fg="#fff", activebackground="#7289da", relief=tk.RAISED, command=self.open_fw_update_popup)
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

    def section_frame(self, title):
        frame = tk.Frame(self, bg="#23272a")
        divider = tk.Frame(self, bg="#36393f", height=2)
        divider.pack(fill="x", padx=5, pady=(10, 0))
        label = tk.Label(frame, text=title, bg="#23272a", fg="#fff", font=("Segoe UI", 11, "bold"))
        label.grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 5))
        return frame

    def info_row(self, parent, label, value, fg="#fff", col=2, pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w", pady=pady)
        val = tk.Label(parent, text=value, bg="#23272a", fg=fg, font=("Segoe UI", 10, "bold"))
        val.grid(row=row, column=col, sticky="e", pady=pady)
        return val

    def health_text_row(self, parent, label, percent, text, color="#43d17a", col=2, pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w", pady=pady)
        label2 = tk.Label(parent, text=f"{text} ({percent}%)", bg="#23272a", fg=color, font=("Segoe UI", 10, "bold"))
        label2.grid(row=row, column=col, sticky="e", padx=5, pady=pady)
        return label2

    def health_row_3col(self, parent, label, percent, text, color="#43d17a", pady=2):
        row = parent.grid_size()[1]
        tk.Label(parent, text=label, bg="#23272a", fg="#aaa").grid(row=row, column=0, sticky="w", pady=pady)
        bar = ttk.Progressbar(parent, value=percent, maximum=100, length=90)
        bar.grid(row=row, column=1, sticky="ew", padx=(0, 5), pady=pady)
        bar_style = ttk.Style()
        bar_style.theme_use('default')
        bar_style.configure(f"{label}_bar.Horizontal.TProgressbar", troughcolor="#2d333b", background=color, bordercolor="#23272a", lightcolor=color, darkcolor=color)
        bar.configure(style=f"{label}_bar.Horizontal.TProgressbar")
        label2 = tk.Label(parent, text=text, bg="#23272a", fg=color, font=("Segoe UI", 10, "bold"))
        label2.grid(row=row, column=2, sticky="e", padx=5, pady=pady)
        return label2, bar

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

    def open_fw_update_popup(self):
        popup = tk.Toplevel(self)
        popup.title("Firmware Update")
        popup.configure(bg="#23272a")
        tk.Label(popup, text="Release:", bg="#23272a", fg="#fff").pack(padx=10, pady=(10, 2), anchor="w")
        release_var = tk.StringVar(value="v1.2.3")
        option_var = tk.StringVar(value="Newest")
        option_menu = tk.OptionMenu(popup, option_var, "Newest", "Other")
        option_menu.config(bg="#2d333b", fg="#fff", highlightthickness=0, activebackground="#7289da")
        option_menu.pack(padx=10, pady=2, fill="x")
        entry = tk.Entry(popup, textvariable=release_var, bg="#2d333b", fg="#888", insertbackground="#fff", state="disabled")
        entry.pack(padx=10, pady=2, fill="x")
        btn_frame = tk.Frame(popup, bg="#23272a")
        btn_frame.pack(padx=10, pady=10, fill="x")
        def on_option_change(*args):
            if option_var.get() == "Other":
                entry.config(state="normal", fg="#fff")
            else:
                entry.config(state="disabled", fg="#888")
        option_var.trace_add("write", on_option_change)
        def do_update():
            # Here you would trigger the real update
            self.last_update.config(text=datetime.datetime.now().strftime("%Y-%m-%d %H:%M"))
            popup.destroy()
        tk.Button(btn_frame, text="Update", command=do_update, bg="#43d17a", fg="#fff").pack(side="left", expand=True, fill="x", padx=(0,5))
        tk.Button(btn_frame, text="Cancel", command=popup.destroy, bg="#2d333b", fg="#fff").pack(side="left", expand=True, fill="x", padx=(5,0))
        entry.focus_set() 