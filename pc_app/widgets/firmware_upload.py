import tkinter as tk
from tkinter import ttk
import time

class FirmwareUpload:
    def __init__(self, protocol, file):
        self.protocol = protocol
        self.is_cancelled = False  # Flag to track cancellation
        self.current_step = 0
        self.total_steps = 8
        self.progress_window = None
        self.progress_label = None
        self.progress_bar = None
        self.cancel_button = None

    def start(self):
        self.create_progress_window()
        self.reset_device()

    def create_progress_window(self):
        # Create the popup window with a progress bar and Cancel button
        self.progress_window = tk.Toplevel()
        self.progress_window.title("Firmware Upload Progress")
        self.progress_label = ttk.Label(self.progress_window, text="Starting firmware upload...")
        self.progress_label.pack(pady=10)
        self.progress_bar = ttk.Progressbar(self.progress_window, orient='horizontal', length=300, mode='determinate')
        self.progress_bar.pack(pady=10)
        self.progress_bar['value'] = 0  # Initial value

        self.cancel_button = ttk.Button(self.progress_window, text="Cancel", command=self.cancel_upload)
        self.cancel_button.pack(pady=5)

    def cancel_upload(self):
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

    def reset_device(self):
        try:
            def callback(ret):
                if self.is_cancelled:
                    return
                if not ret:
                    self.reportError("Problem with resetting device...")
                    return
                
                print("reset callback")
                self.wait_for_bootloader()

            self.update_progress("Resetting device...")
            self.protocol.reset(callback)
        except Exception as e:
            print(f"Error resetting device: {e}")
            self.reportError(f"Error resetting device: {e}")
            
    def wait_for_bootlader(self):
        time.sleep(10)
        pass