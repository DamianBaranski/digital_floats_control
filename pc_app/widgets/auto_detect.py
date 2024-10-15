import tkinter as tk
from tkinter import ttk
from .channel_settings import ChannelSettings

class AutoDetect:
    def __init__(self, protocol, channel_settings_table):
        self.protocol = protocol
        self.channel_settings_table = channel_settings_table
        self.ina_addr = []
        self.pcf_addr = []
        self.old_settings = []
        self.new_settings = []
        self.is_cancelled = False  # Flag to track cancellation
        self.current_step = 0
        self.total_steps = 8
        self.progress_window = None
        self.progress_label = None
        self.progress_bar = None
        self.cancel_button = None

    def start(self):
        self.create_progress_window()
        self.detect_ina()

    def create_progress_window(self):
        # Create the popup window with a progress bar and Cancel button
        self.progress_window = tk.Toplevel()
        self.progress_window.title("Auto Detection Progress")
        self.progress_label = ttk.Label(self.progress_window, text="Starting detection...")
        self.progress_label.pack(pady=10)
        self.progress_bar = ttk.Progressbar(self.progress_window, orient='horizontal', length=300, mode='determinate')
        self.progress_bar.pack(pady=10)
        self.progress_bar['value'] = 0  # Initial value

        self.cancel_button = ttk.Button(self.progress_window, text="Cancel", command=self.cancel_detection)
        self.cancel_button.pack(pady=5)

    def cancel_detection(self):
        self.is_cancelled = True
        self.progress_window.destroy()  # Close the window on cancel

    def update_progress(self, step_description):
        if self.is_cancelled:
            return  # Skip updates if cancelled
        self.current_step += 1
        self.progress_label.config(text=step_description)
        self.progress_bar['value'] = (self.current_step / self.total_steps) * 100
        if self.current_step >= self.total_steps:
            self.complete_process()
            
    def reportError(self, text):
        self.progress_label.config(text=text)
        self.cancel_button.config(text="OK")

    def complete_process(self):
        # Replace Cancel button with OK button when finished
        self.cancel_button.pack_forget()
        ok_button = ttk.Button(self.progress_window, text="OK", command=self.progress_window.destroy)
        ok_button.pack(pady=5)

    def detect_ina(self):
        try:
            print("Detect INA addr")
            ina_start_address = 0x40
            max_no_ina_sensors = 16

            def ina_callback(data, address):
                if self.is_cancelled:
                    return
                self.ina_addr.append((address, data))
                print("INA callback:", (address, data))
                
                if len(self.ina_addr) >= max_no_ina_sensors:
                    print("Call detect PCF:", len(self.ina_addr), max_no_ina_sensors)
                    self.detect_pcf()
                else:
                    self.protocol.scanI2c(address + 1, lambda data, addr=address + 1: ina_callback(data, addr))

            self.update_progress("Detecting INA sensors...")
            self.protocol.scanI2c(ina_start_address, lambda data, addr=ina_start_address: ina_callback(data, addr))
        except Exception as e:
            print(f"Error detecting INA: {e}")
            self.reportError(f"Error detecting INA: {e}")

    def detect_pcf(self):
        try:
            pcf_start_address = 0x20
            max_no_pcf_expanders = 8

            def pcf_callback(data, address):
                if self.is_cancelled:
                    return
                self.pcf_addr.append((address, data))
                print("PCF callback:", (address, data))
                if len(self.pcf_addr) == max_no_pcf_expanders:
                    print("Call get settings:", len(self.pcf_addr), max_no_pcf_expanders)
                    self.get_settings()

            self.update_progress("Detecting PCF expanders...")
            for i in range(pcf_start_address, pcf_start_address + max_no_pcf_expanders):
                self.protocol.scanI2c(i, lambda data, addr=i: pcf_callback(data, addr))
        except Exception as e:
            print(f"Error detecting PCF: {e}")
            self.reportError(f"Error detecting PCF: {e}")

    def get_settings(self):
        try:
            print("Get settings")
            status = [False] * 6

            def callback(channel_settings, idx):
                if self.is_cancelled:
                    return
                print("Get settings callback:", idx)
                status[idx] = True
                self.old_settings.append(channel_settings)
                if all(status):
                    self.clean_settings()

            self.update_progress("Fetching settings...")
            for i in range(6):
                self.protocol.getChannelSettings(i, lambda data, idx=i: callback(data, idx))
        except Exception as e:
            print(f"Error fetching settings: {e}")
            self.reportError(f"Error fetching settings: {e}")

    def clean_settings(self):
        try:
            print("Disable channels")
            status = [False] * 6

            def callback(ret, idx):
                if self.is_cancelled:
                    return
                print("Disable channels callback:", idx)
                status[idx] = True
                if all(status):
                    print("Call test relays")
                    self.test_relays()

            self.update_progress("Disabling channels...")
            for i in range(6):
                self.protocol.updateChannelSettings(i, ChannelSettings(), lambda ret, idx=i: callback(ret, idx))
        except Exception as e:
            print(f"Error disabling channels: {e}")
            self.reportError(f"Error disabling channels: {e}")

    def test_relays(self):
        try:
            print("Test relays")
            self.update_progress("Testing relays...")
            tmp_ina = [item[0] for item in self.ina_addr if item[1] == True]
            tmp_pcf = [item[0] for item in self.pcf_addr if item[1] == True]
            self.ina_addr[:] = tmp_ina
            self.pcf_addr[:] = tmp_pcf
            ina_idx = 0
            pcf_idx = 0
            pcf_channel = 0

            def next_pcf():
                nonlocal pcf_channel, pcf_idx, ina_idx
                if pcf_channel == 0:
                    pcf_channel = 1
                else:
                    pcf_idx += 1
                    pcf_channel = 0
                ina_idx = 0

            def callback(ret):
                nonlocal pcf_channel, pcf_idx, ina_idx
                if self.is_cancelled:
                    return
                print(f"Test relay callback pcf_addr: {self.pcf_addr[pcf_idx]} pcf_channel: {pcf_channel} ina_addr: {self.ina_addr[ina_idx]} ret: {ret}")

                if ret:
                    settings = ChannelSettings()
                    settings.values['pcf_addr'] = self.pcf_addr[pcf_idx]
                    settings.values['pcf_channel'] = pcf_channel
                    settings.values['ina_addr'] = self.ina_addr[ina_idx]
                    self.new_settings.append(settings)
                    self.ina_addr.pop(ina_idx)
                    next_pcf()
                else:
                    ina_idx += 1

                if ina_idx >= len(self.ina_addr):
                    next_pcf()

                if pcf_idx >= len(self.pcf_addr):
                    print("All PCF addresses tested.")
                    for s in self.new_settings:
                        print(s)
                    self.restore_settings()
                    return

                if pcf_idx < len(self.pcf_addr) and ina_idx < len(self.ina_addr):
                    self.protocol.testRelays(self.pcf_addr[pcf_idx], pcf_channel, self.ina_addr[ina_idx], callback)

            if len(self.pcf_addr) > 0 and len(self.ina_addr) > 0:
                self.protocol.testRelays(self.pcf_addr[pcf_idx], pcf_channel, self.ina_addr[ina_idx], callback)
            else:
                print("No channels detected.")
        except Exception as e:
            print(f"Error testing relays: {e}")
            self.reportError(f"Error testing relays: {e}")

    def generate_new_settings(self):
        try:
            print("Generate new settings")
            for (idx, s) in enumerate(self.new_settings):
                s.values['max_voltage_limit'] = 260
                s.values['min_voltage_limit'] = 80
                s.values['max_current_limit'] = 50
                s.values['min_current_limit'] = 0
                s.values['ina_calibration'] = 50
                s.values['enable'] = True
                self.channel_settings_table.addData(idx)
                self.channel_settings_table.setData(idx, s)
            self.channel_settings_table.populate_treeview()
            self.update_progress("Completed")
        except Exception as e:
            print(f"Error generating new settings: {e}")
            self.reportError(f"Error generating new settings: {e}")
            
    def restore_settings(self):
        """Restore the settings that were obtained before relay testing."""
        try:
            print("Restoring old settings...")
            self.update_progress("Restoring previous settings...")

            status = [False] * 6  # Track status of restoration

            def callback(ret, idx):
                print(f"Restoring settings for channel {idx}, result: {ret}")
                status[idx] = True
                if all(status):
                    print("All settings restored successfully.")
                    self.update_progress("Settings restored.")
                    self.generate_new_settings()

            for idx, settings in enumerate(self.old_settings):
                self.protocol.updateChannelSettings(idx, settings, lambda ret, idx=idx: callback(ret, idx))

        except Exception as e:
            print(f"Error restoring settings: {e}")
            self.progress_label.config(text=f"Error restoring settings: {e}")
