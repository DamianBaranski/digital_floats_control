import tkinter as tk
from tkinter import ttk
import time
import multiprocessing

class FirmwareUpload:
    def __init__(self, protocol, file):
        self.protocol = protocol
        self.file = file
        self.is_cancelled = multiprocessing.Value('b', False)  # Flag to track cancellation (shared between processes)
        self.current_step = multiprocessing.Value('i', 0)  # Shared counter for progress
        self.total_steps = 8
        self.progress_window = None
        self.progress_label = None
        self.progress_bar = None
        self.cancel_button = None
        self.finished_callback = None  # Callback to signal completion
        self.queue = multiprocessing.Queue()  # Queue for inter-process communication

    def start(self, callback=None):
        self.finished_callback = callback  # Store the callback function
        self.create_progress_window()
        self.process = multiprocessing.Process(target=self.upload_process)
        self.process.start()
        self.monitor_queue()  # Monitor the queue for progress updates

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
        with self.is_cancelled.get_lock():
            self.is_cancelled.value = True  # Set the cancellation flag in the process-safe way
        self.progress_window.destroy()  # Close the window on cancel

    def update_progress(self, step_description):
        self.progress_label.config(text=step_description)
        self.progress_bar['value'] = (self.current_step.value / self.total_steps) * 100

    def reportError(self, text):
        self.progress_label.config(text=text)
        self.cancel_button.config(text="OK")

    def complete_process(self):
        def callback():
            if self.finished_callback:
                self.finished_callback(True)  # Call the callback with a success flag
            self.progress_window.destroy()
        
        # Replace Cancel button with OK button when finished
        self.cancel_button.pack_forget()
        ok_button = ttk.Button(self.progress_window, text="OK", command=callback)
        ok_button.pack(pady=5)

    def monitor_queue(self):
        try:
            message = self.queue.get_nowait()
        except multiprocessing.queues.Empty:
            self.progress_window.after(100, self.monitor_queue)  # Check again after 100ms
            return

        if isinstance(message, str):
            self.update_progress(message)
            self.progress_window.after(100, self.monitor_queue)
        elif isinstance(message, bool):
            if message:
                self.complete_process()
            else:
                self.reportError("Upload failed")

    def upload_process(self):
        # Upload process running in the separate process
        try:
            self.queue.put("Resetting device...")
            self.reset_device()
        except Exception as e:
            self.queue.put(f"Error: {e}")
            self.queue.put(False)  # Indicate failure

    def reset_device(self):
        def callback(ret):
            print("reset_device callback:", ret)
            if self.is_cancelled.value:
                return
            if not ret:
                self.queue.put("Problem with resetting device...")
                self.queue.put(False)
            else:
                print("Device reset successful.")
                self.queue.put("Device reset successful.")
                self.wait_for_bootloader()

        print("Call protocol reset")
        time.sleep(3)
        self.protocol.reset(callback)

    def wait_for_bootloader(self):
        try_again = True
        self.queue.put("Waiting for bootloader...")
        def callback(ret):
            print(ret)
            if "Boot" in ret:
                try_again = False
                self.queue.put("Bootloader found. Continuing...")
                self.complete_process()

        while True:
            print("Calling getVersion")
            self.protocol.getVersion(lambda version: callback(version))
            time.sleep(1)

