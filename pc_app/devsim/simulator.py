#!/usr/bin/env python3
"""
Device Simulator for Digital Floats Control.
Creates a virtual COM port and simulates device responses.
"""
import os
import pty
import tty
import threading
import time
import tkinter as tk
from tkinter import ttk, messagebox
from typing import Optional, Callable, Dict

from protocol import (
    Protocol,
    StatusDataEncoder,
    FirmwareInfoEncoder,
    MonitoringDataEncoder,
    ChannelSettingsEncoder,
    ErrorStatusEncoder,
    RemoteControlDataDecoder,
)


class VirtualSerialPort:
    """Virtual serial port using PTY (pseudo-terminal)."""
    
    def __init__(self):
        self.master_fd: Optional[int] = None
        self.slave_fd: Optional[int] = None
        self.slave_name: Optional[str] = None
        self._running = False
        self._thread: Optional[threading.Thread] = None
        self._handlers: Dict[str, Callable] = {}
        self._log_callback: Optional[Callable] = None
        
    def start(self) -> str:
        """Start the virtual serial port. Returns the slave device path."""
        self.master_fd, self.slave_fd = pty.openpty()
        self.slave_name = os.ttyname(self.slave_fd)
        
        # Set raw mode on master
        tty.setraw(self.master_fd)
        
        # Configure slave terminal settings (important for serial communication)
        import termios
        import struct
        
        # Get current termios settings for slave
        slave_settings = termios.tcgetattr(self.slave_fd)
        
        # Set to raw mode (disable echo, canonical mode, etc.)
        slave_settings[0] = slave_settings[0] & ~(termios.IGNBRK | termios.BRKINT | termios.PARMRK | 
                                                   termios.ISTRIP | termios.INLCR | termios.IGNCR | 
                                                   termios.ICRNL | termios.IXON)
        slave_settings[1] = slave_settings[1] & ~termios.OPOST
        slave_settings[2] = slave_settings[2] & ~(termios.CSIZE | termios.PARENB)
        slave_settings[2] = slave_settings[2] | termios.CS8
        slave_settings[3] = slave_settings[3] & ~(termios.ECHO | termios.ECHONL | termios.ICANON | 
                                                   termios.ISIG | termios.IEXTEN)
        slave_settings[4] = termios.B115200  # Baud rate
        slave_settings[5] = termios.B115200
        
        termios.tcsetattr(self.slave_fd, termios.TCSANOW, slave_settings)
        
        print(f"DEBUG: Created PTY - Master: {self.master_fd}, Slave: {self.slave_name}")
        
        self._running = True
        self._thread = threading.Thread(target=self._read_loop, daemon=True)
        self._thread.start()
        
        return self.slave_name
    
    def stop(self):
        """Stop the virtual serial port."""
        self._running = False
        if self._thread:
            self._thread.join(timeout=1.0)
        if self.master_fd:
            os.close(self.master_fd)
        if self.slave_fd:
            os.close(self.slave_fd)
        self.master_fd = None
        self.slave_fd = None
        
    def register_handler(self, cmd: str, handler: Callable):
        """Register a handler for a specific command."""
        self._handlers[cmd] = handler
        
    def set_log_callback(self, callback: Callable):
        """Set callback for logging received/sent data."""
        self._log_callback = callback
        
    def _log(self, direction: str, data: bytes):
        """Log data transfer."""
        if self._log_callback:
            self._log_callback(direction, data)
    
    def _read_loop(self):
        """Main loop reading from the virtual port."""
        buffer = b''
        
        while self._running:
            try:
                # Non-blocking read with select
                import select
                readable, _, _ = select.select([self.master_fd], [], [], 0.1)
                
                if not readable:
                    continue
                    
                chunk = os.read(self.master_fd, 1024)
                if not chunk:
                    continue
                
                print(f"DEBUG: Read {len(chunk)} bytes from master_fd: {chunk.hex()}")
                buffer += chunk
                
                # Process complete messages (terminated by \r)
                while b'\r' in buffer:
                    idx = buffer.index(b'\r')
                    message = buffer[:idx + 1]
                    buffer = buffer[idx + 1:]
                    
                    self._handle_message(message)
                    
            except OSError as e:
                print(f"OSError in read loop: {e}")
                break
            except Exception as e:
                print(f"Read error: {e}")
                import traceback
                traceback.print_exc()
                
    def _handle_message(self, raw_data: bytes):
        """Handle an incoming message."""
        self._log("RX", raw_data)
        
        result = Protocol.decode_request(raw_data)
        if result is None:
            return
            
        cmd, payload = result
        
        handler = self._handlers.get(cmd)
        if handler:
            response_data = handler(cmd, payload)
            if response_data is not None:
                response = Protocol.encode_response(cmd, response_data)
                self._send(response)
        else:
            # Unknown command - send empty response
            response = Protocol.encode_response(cmd, b'\x00')
            self._send(response)
            
    def _send(self, data: bytes):
        """Send data to the virtual port."""
        if self.master_fd:
            try:
                written = os.write(self.master_fd, data)
                # Force flush by using fsync on the file descriptor
                import fcntl
                fcntl.fsync(self.master_fd)
                self._log("TX", data)
                print(f"DEBUG: Wrote {written} bytes to master_fd: {data.hex()}")
            except OSError as e:
                print(f"Send error: {e}")
            except Exception as e:
                print(f"Unexpected send error: {e}")


class StatusDataTab(ttk.Frame):
    """Tab for StatusData simulation."""
    
    def __init__(self, parent, on_data_change: Callable):
        super().__init__(parent)
        self.on_data_change = on_data_change
        
        # Data values
        self.power_voltage = tk.DoubleVar(value=12.0)
        self.memory_usage = tk.IntVar(value=1024)
        self.uptime = tk.IntVar(value=0)
        self.ldg_gear_switch = tk.BooleanVar(value=False)
        self.rudder_switch = tk.BooleanVar(value=False)
        self.test_button = tk.BooleanVar(value=False)
        self.remote_control = tk.BooleanVar(value=False)
        
        # Auto-increment uptime
        self.auto_uptime = tk.BooleanVar(value=True)
        self._uptime_base = time.time()
        
        self._build_ui()
        self._start_uptime_update()
        
    def _build_ui(self):
        """Build the tab UI."""
        # Main container with padding
        container = ttk.Frame(self, padding=20)
        container.pack(fill=tk.BOTH, expand=True)
        
        # Title
        title = ttk.Label(container, text="Status Data Simulator",
                         font=('Segoe UI', 16, 'bold'))
        title.pack(pady=(0, 20))
        
        # Values frame
        values_frame = ttk.LabelFrame(container, text="Analog Values", padding=15)
        values_frame.pack(fill=tk.X, pady=(0, 15))
        
        # Power Voltage
        volt_frame = ttk.Frame(values_frame)
        volt_frame.pack(fill=tk.X, pady=5)
        ttk.Label(volt_frame, text="Power Voltage (V):", width=20).pack(side=tk.LEFT)
        volt_spin = ttk.Spinbox(volt_frame, from_=0, to=30, increment=0.1,
                                textvariable=self.power_voltage, width=15)
        volt_spin.pack(side=tk.LEFT, padx=5)
        volt_scale = ttk.Scale(volt_frame, from_=0, to=30, orient=tk.HORIZONTAL,
                              variable=self.power_voltage, length=200)
        volt_scale.pack(side=tk.LEFT, padx=10)
        
        # Memory Usage
        mem_frame = ttk.Frame(values_frame)
        mem_frame.pack(fill=tk.X, pady=5)
        ttk.Label(mem_frame, text="Memory Usage (KB):", width=20).pack(side=tk.LEFT)
        mem_spin = ttk.Spinbox(mem_frame, from_=0, to=65535, increment=64,
                               textvariable=self.memory_usage, width=15)
        mem_spin.pack(side=tk.LEFT, padx=5)
        mem_scale = ttk.Scale(mem_frame, from_=0, to=8192, orient=tk.HORIZONTAL,
                             variable=self.memory_usage, length=200)
        mem_scale.pack(side=tk.LEFT, padx=10)
        
        # Uptime
        uptime_frame = ttk.Frame(values_frame)
        uptime_frame.pack(fill=tk.X, pady=5)
        ttk.Label(uptime_frame, text="Uptime (ms):", width=20).pack(side=tk.LEFT)
        self.uptime_label = ttk.Label(uptime_frame, text="0", width=15,
                                      font=('Consolas', 10))
        self.uptime_label.pack(side=tk.LEFT, padx=5)
        self.auto_uptime_check = ttk.Checkbutton(uptime_frame, text="Auto-increment",
                                                 variable=self.auto_uptime,
                                                 command=self._on_auto_uptime_toggle)
        self.auto_uptime_check.pack(side=tk.LEFT, padx=10)
        ttk.Button(uptime_frame, text="Reset", command=self._reset_uptime,
                  width=8).pack(side=tk.LEFT, padx=5)
        
        # Switches frame
        switches_frame = ttk.LabelFrame(container, text="Switches", padding=15)
        switches_frame.pack(fill=tk.X, pady=(0, 15))
        
        # Create switches in a grid
        switches_inner = ttk.Frame(switches_frame)
        switches_inner.pack()
        
        # Landing Gear Switch
        ldg_check = ttk.Checkbutton(switches_inner, text="Landing Gear Switch",
                                    variable=self.ldg_gear_switch,
                                    style='Switch.TCheckbutton')
        ldg_check.grid(row=0, column=0, padx=20, pady=5, sticky='w')
        
        # Rudder Switch
        rudder_check = ttk.Checkbutton(switches_inner, text="Rudder Switch",
                                       variable=self.rudder_switch,
                                       style='Switch.TCheckbutton')
        rudder_check.grid(row=0, column=1, padx=20, pady=5, sticky='w')
        
        # Test Button
        test_check = ttk.Checkbutton(switches_inner, text="Test Button",
                                     variable=self.test_button,
                                     style='Switch.TCheckbutton')
        test_check.grid(row=1, column=0, padx=20, pady=5, sticky='w')
        
        # Remote Control Status
        remote_check = ttk.Checkbutton(switches_inner, text="Remote Control Active",
                                       variable=self.remote_control,
                                       style='Switch.TCheckbutton')
        remote_check.grid(row=1, column=1, padx=20, pady=5, sticky='w')
        
        # Quick presets
        presets_frame = ttk.LabelFrame(container, text="Quick Presets", padding=15)
        presets_frame.pack(fill=tk.X, pady=(0, 15))
        
        preset_buttons = ttk.Frame(presets_frame)
        preset_buttons.pack()
        
        ttk.Button(preset_buttons, text="Normal Operation",
                  command=self._preset_normal).pack(side=tk.LEFT, padx=5)
        ttk.Button(preset_buttons, text="Low Voltage",
                  command=self._preset_low_voltage).pack(side=tk.LEFT, padx=5)
        ttk.Button(preset_buttons, text="All Switches ON",
                  command=self._preset_all_on).pack(side=tk.LEFT, padx=5)
        ttk.Button(preset_buttons, text="All Switches OFF",
                  command=self._preset_all_off).pack(side=tk.LEFT, padx=5)
        
        # Protocol preview
        preview_frame = ttk.LabelFrame(container, text="Protocol Data Preview", padding=15)
        preview_frame.pack(fill=tk.BOTH, expand=True)
        
        self.preview_text = tk.Text(preview_frame, height=6, font=('Consolas', 10),
                                   bg='#1a1a2e', fg='#0fe0a0', insertbackground='white')
        self.preview_text.pack(fill=tk.BOTH, expand=True)
        
        # Bind variable changes to update preview
        for var in [self.power_voltage, self.memory_usage, self.ldg_gear_switch,
                   self.rudder_switch, self.test_button, self.remote_control]:
            var.trace_add('write', self._update_preview)
        
        self._update_preview()
        
    def _start_uptime_update(self):
        """Start the uptime auto-increment."""
        def update():
            if self.auto_uptime.get():
                elapsed_ms = int((time.time() - self._uptime_base) * 1000)
                self.uptime.set(elapsed_ms)
                self.uptime_label.config(text=f"{elapsed_ms:,}")
            self.after(100, update)
        update()
        
    def _on_auto_uptime_toggle(self):
        """Handle auto-uptime toggle."""
        if self.auto_uptime.get():
            self._uptime_base = time.time() - (self.uptime.get() / 1000)
            
    def _reset_uptime(self):
        """Reset uptime to 0."""
        self._uptime_base = time.time()
        self.uptime.set(0)
        self.uptime_label.config(text="0")
        
    def _update_preview(self, *args):
        """Update the protocol data preview."""
        data = self.get_encoded_data()
        
        self.preview_text.delete('1.0', tk.END)
        
        # Show raw bytes
        hex_str = ' '.join(f'{b:02X}' for b in data)
        self.preview_text.insert(tk.END, f"Raw bytes ({len(data)} bytes):\n")
        self.preview_text.insert(tk.END, f"  {hex_str}\n\n")
        
        # Show interpreted values
        self.preview_text.insert(tk.END, "Interpreted:\n")
        self.preview_text.insert(tk.END, f"  Voltage: {self.power_voltage.get():.1f}V | ")
        self.preview_text.insert(tk.END, f"Memory: {self.memory_usage.get()} KB | ")
        self.preview_text.insert(tk.END, f"Uptime: {self.uptime.get()} ms\n")
        
        switches = []
        if self.ldg_gear_switch.get(): switches.append("LDG")
        if self.rudder_switch.get(): switches.append("RUD")
        if self.test_button.get(): switches.append("TEST")
        if self.remote_control.get(): switches.append("RMT")
        self.preview_text.insert(tk.END, f"  Switches: {', '.join(switches) if switches else 'None'}")
        
    def _preset_normal(self):
        """Apply normal operation preset."""
        self.power_voltage.set(12.0)
        self.memory_usage.set(1024)
        self.ldg_gear_switch.set(False)
        self.rudder_switch.set(False)
        self.test_button.set(False)
        self.remote_control.set(False)
        
    def _preset_low_voltage(self):
        """Apply low voltage preset."""
        self.power_voltage.set(9.5)
        
    def _preset_all_on(self):
        """Turn all switches on."""
        self.ldg_gear_switch.set(True)
        self.rudder_switch.set(True)
        self.test_button.set(True)
        self.remote_control.set(True)
        
    def _preset_all_off(self):
        """Turn all switches off."""
        self.ldg_gear_switch.set(False)
        self.rudder_switch.set(False)
        self.test_button.set(False)
        self.remote_control.set(False)
        
    def get_encoded_data(self) -> bytes:
        """Get the current data encoded as protocol bytes."""
        return StatusDataEncoder.encode(
            power_voltage=self.power_voltage.get(),
            memory_usage=self.memory_usage.get(),
            uptime=self.uptime.get(),
            ldg_gear_switch=self.ldg_gear_switch.get(),
            rudder_switch=self.rudder_switch.get(),
            test_button=self.test_button.get(),
            remote_control_status=self.remote_control.get()
        )


class DeviceSimulatorApp:
    """Main Device Simulator Application."""
    
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Digital Floats - Device Simulator")
        self.root.geometry("900x700")
        self.root.minsize(800, 600)
        
        # Virtual port
        self.vport = VirtualSerialPort()
        self.port_path: Optional[str] = None
        
        # Apply theme
        self._setup_theme()
        
        # Build UI
        self._build_ui()
        
        # Start virtual port
        self._start_port()
        
        # Handle close
        self.root.protocol("WM_DELETE_WINDOW", self._on_close)
        
    def _setup_theme(self):
        """Setup the application theme."""
        style = ttk.Style()
        
        # Try to use a modern theme
        available = style.theme_names()
        if 'clam' in available:
            style.theme_use('clam')
        
        # Custom colors
        bg_dark = '#1a1a2e'
        bg_medium = '#16213e'
        accent = '#0f3460'
        highlight = '#e94560'
        text_light = '#eaeaea'
        success = '#0fe0a0'
        
        # Configure styles
        self.root.configure(bg=bg_medium)
        
        style.configure('TFrame', background=bg_medium)
        style.configure('TLabel', background=bg_medium, foreground=text_light)
        style.configure('TLabelframe', background=bg_medium, foreground=text_light)
        style.configure('TLabelframe.Label', background=bg_medium, foreground=highlight,
                       font=('Segoe UI', 10, 'bold'))
        style.configure('TNotebook', background=bg_dark)
        style.configure('TNotebook.Tab', background=accent, foreground=text_light,
                       padding=[15, 8], font=('Segoe UI', 9))
        style.map('TNotebook.Tab',
                 background=[('selected', highlight)],
                 foreground=[('selected', 'white')])
        style.configure('TButton', background=accent, foreground=text_light,
                       padding=[10, 5], font=('Segoe UI', 9))
        style.map('TButton',
                 background=[('active', highlight)])
        style.configure('TCheckbutton', background=bg_medium, foreground=text_light)
        style.configure('Switch.TCheckbutton', background=bg_medium, foreground=text_light,
                       font=('Segoe UI', 10))
        style.configure('TSpinbox', fieldbackground=bg_dark, foreground=text_light)
        style.configure('TScale', background=bg_medium, troughcolor=bg_dark)
        style.configure('Status.TLabel', background=success, foreground=bg_dark,
                       font=('Consolas', 11, 'bold'), padding=[10, 5])
        style.configure('Header.TLabel', font=('Segoe UI', 12, 'bold'),
                       foreground=highlight)
        
    def _build_ui(self):
        """Build the main UI."""
        # Top frame with port info
        top_frame = ttk.Frame(self.root, padding=10)
        top_frame.pack(fill=tk.X)
        
        ttk.Label(top_frame, text="Virtual COM Port:",
                 style='Header.TLabel').pack(side=tk.LEFT)
        self.port_label = ttk.Label(top_frame, text="Not started",
                                   style='Status.TLabel')
        self.port_label.pack(side=tk.LEFT, padx=10)
        
        # Copy button
        ttk.Button(top_frame, text="📋 Copy Path",
                  command=self._copy_port_path).pack(side=tk.LEFT, padx=5)
        
        # Connection indicator
        self.status_indicator = tk.Canvas(top_frame, width=20, height=20,
                                         bg='#16213e', highlightthickness=0)
        self.status_indicator.pack(side=tk.RIGHT, padx=10)
        self._draw_status_indicator(False)
        
        ttk.Label(top_frame, text="Activity:").pack(side=tk.RIGHT)
        
        # Notebook for tabs
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Status Data tab
        self.status_tab = StatusDataTab(self.notebook, self._on_data_change)
        self.notebook.add(self.status_tab, text="  Status Data  ")
        
        # Placeholder tabs for other data types
        for tab_name in ["Firmware Info", "Monitoring", "Channel Settings", 
                        "Error Status", "Remote Control"]:
            placeholder = ttk.Frame(self.notebook, padding=40)
            label = ttk.Label(placeholder, 
                            text=f"{tab_name} simulation\n(Coming soon)",
                            font=('Segoe UI', 14), justify='center')
            label.pack(expand=True)
            self.notebook.add(placeholder, text=f"  {tab_name}  ")
        
        # Log panel at bottom
        log_frame = ttk.LabelFrame(self.root, text="Communication Log", padding=5)
        log_frame.pack(fill=tk.X, padx=10, pady=(0, 10))
        
        log_container = ttk.Frame(log_frame)
        log_container.pack(fill=tk.X)
        
        self.log_text = tk.Text(log_container, height=4, font=('Consolas', 9),
                               bg='#0d0d1a', fg='#8888aa', insertbackground='white')
        self.log_text.pack(side=tk.LEFT, fill=tk.X, expand=True)
        
        scrollbar = ttk.Scrollbar(log_container, orient=tk.VERTICAL,
                                 command=self.log_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.log_text.config(yscrollcommand=scrollbar.set)
        
        # Clear log button
        ttk.Button(log_frame, text="Clear Log",
                  command=self._clear_log).pack(anchor='e', pady=5)
        
    def _start_port(self):
        """Start the virtual serial port."""
        try:
            self.port_path = self.vport.start()
            self.port_label.config(text=self.port_path)
            
            # Register command handlers
            self.vport.register_handler(Protocol.CMD_STATUS, self._handle_status)
            self.vport.register_handler(Protocol.CMD_FIRMWARE_INFO, self._handle_firmware_info)
            self.vport.register_handler(Protocol.CMD_MONITORING, self._handle_monitoring)
            self.vport.register_handler(Protocol.CMD_CHANNEL_SETTINGS, self._handle_channel_settings)
            self.vport.register_handler(Protocol.CMD_ERROR_STATUS, self._handle_error_status)
            self.vport.register_handler(Protocol.CMD_REMOTE_CONTROL, self._handle_remote_control)
            
            # Set log callback
            self.vport.set_log_callback(self._log_message)
            
            self._log("System", f"Virtual port started: {self.port_path}")
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to create virtual port: {e}")
            
    def _handle_status(self, cmd: str, payload: bytes) -> bytes:
        """Handle StatusData request."""
        self._flash_activity()
        return self.status_tab.get_encoded_data()
    
    def _handle_firmware_info(self, cmd: str, payload: bytes) -> bytes:
        """Handle FirmwareInfo request - placeholder."""
        self._flash_activity()
        return FirmwareInfoEncoder.encode(
            app_version="1.0.0-sim",
            hardware_version="SIM-1.0",
            serial_number="DEVSIM-001",
            build_date="2026-01-03",
            build_time="12:00:00",
            git_commit="simulator"
        )
    
    def _handle_monitoring(self, cmd: str, payload: bytes) -> bytes:
        """Handle Monitoring request - placeholder."""
        self._flash_activity()
        channel = payload[0] if payload else 0
        return MonitoringDataEncoder.encode(
            timestamp=self.status_tab.uptime.get(),
            current_ma=100,
            channel=channel,
            state=MonitoringDataEncoder.STATE_UP,
            up_switch=True,
            down_switch=False
        )
    
    def _handle_channel_settings(self, cmd: str, payload: bytes) -> bytes:
        """Handle ChannelSettings request - placeholder."""
        self._flash_activity()
        channel = payload[0] if payload else 0
        return ChannelSettingsEncoder.encode(
            channel=channel,
            enable=True,
            bridge=False,
            inverse_motor=False,
            inverse_up_limit=False,
            inverse_down_limit=False,
            inverse_limit=False,
            rudder=False,
            timeout=5,
            bridge_channel=0,
            max_current_warning=0.5,
            max_current_error=1.0,
            min_current=0.0
        )
    
    def _handle_error_status(self, cmd: str, payload: bytes) -> bytes:
        """Handle ErrorStatus request - placeholder (no errors)."""
        self._flash_activity()
        return ErrorStatusEncoder.encode(
            channel_errors=[0] * 6,
            channel_warnings=[0] * 6,
            system_warnings=0
        )
    
    def _handle_remote_control(self, cmd: str, payload: bytes) -> bytes:
        """Handle RemoteControl command."""
        self._flash_activity()
        ldg, rudder, test = RemoteControlDataDecoder.decode(payload)
        self._log("RemoteCtrl", f"LDG={ldg}, Rudder={rudder}, Test={test}")
        
        # Update status tab switches based on remote control
        self.status_tab.ldg_gear_switch.set(ldg)
        self.status_tab.rudder_switch.set(rudder)
        self.status_tab.test_button.set(test)
        
        return b'\x01'  # Success
    
    def _flash_activity(self):
        """Flash the activity indicator."""
        self._draw_status_indicator(True)
        self.root.after(100, lambda: self._draw_status_indicator(False))
        
    def _draw_status_indicator(self, active: bool):
        """Draw the status indicator."""
        self.status_indicator.delete('all')
        color = '#0fe0a0' if active else '#333355'
        self.status_indicator.create_oval(3, 3, 17, 17, fill=color, outline='')
        
    def _log_message(self, direction: str, data: bytes):
        """Log a message to the log panel."""
        timestamp = time.strftime("%H:%M:%S")
        
        # Format data for display
        try:
            display = data.decode('utf-8', errors='replace').strip()[:60]
        except:
            display = data.hex()[:60]
        
        color = '#0fe0a0' if direction == 'TX' else '#e94560'
        
        self.log_text.insert(tk.END, f"[{timestamp}] ")
        self.log_text.insert(tk.END, f"{direction}: ", f"tag_{direction}")
        self.log_text.insert(tk.END, f"{display}\n")
        self.log_text.see(tk.END)
        
        # Configure tags
        self.log_text.tag_config('tag_TX', foreground='#0fe0a0')
        self.log_text.tag_config('tag_RX', foreground='#e94560')
        
    def _log(self, category: str, message: str):
        """Log a system message."""
        timestamp = time.strftime("%H:%M:%S")
        self.log_text.insert(tk.END, f"[{timestamp}] [{category}] {message}\n")
        self.log_text.see(tk.END)
        
    def _clear_log(self):
        """Clear the log panel."""
        self.log_text.delete('1.0', tk.END)
        
    def _copy_port_path(self):
        """Copy the port path to clipboard."""
        if self.port_path:
            self.root.clipboard_clear()
            self.root.clipboard_append(self.port_path)
            self._log("System", "Port path copied to clipboard")
            
    def _on_data_change(self):
        """Called when tab data changes."""
        pass  # Future: Could trigger auto-send or preview update
        
    def _on_close(self):
        """Handle window close."""
        self.vport.stop()
        self.root.destroy()
        
    def run(self):
        """Run the application."""
        self.root.mainloop()


def main():
    app = DeviceSimulatorApp()
    app.run()


if __name__ == "__main__":
    main()

