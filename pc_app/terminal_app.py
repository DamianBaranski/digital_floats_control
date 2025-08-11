import sys
import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests

class DigitalFloatsTerminalApp:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.subscribe(requests.FirmwareVersionRequest(), self.firmware_version_update)
        self.device_client.subscribe(requests.StatusRequest(), self.status_update)

        for i in range(6):
            self.device_client.subscribe(
                requests.MonitoringChannelRequest(i),
                lambda data, ch=i: self.monitoring_update(ch, data)
            )

        self.device_client.connect(port)

        # Store last values
        self.firmware_version = None
        self.status = None
        self.monitoring_data = {i: None for i in range(4)}

    def redraw(self):
        # Move cursor to top-left and clear screen
        sys.stdout.write("\033[H\033[J")
        sys.stdout.write("=== Digital Floats Terminal ===\n")
        sys.stdout.write(f"Firmware Version : {self.firmware_version}\n")
        sys.stdout.write(f"Status           : {self.status}\n")
        sys.stdout.write("\nMonitoring Channels:\n")
        for ch, value in self.monitoring_data.items():
            sys.stdout.write(f"  Ch {ch} : {value}\n")
        sys.stdout.flush()

    def status_update(self, status):
        self.status = status

    def firmware_version_update(self, version):
        self.firmware_version = version

    def monitoring_update(self, channel, data):
        self.monitoring_data[channel] = data

if __name__ == "__main__":
    app = DigitalFloatsTerminalApp("/dev/ttyUSB0")
    try:
        while True:
            app.redraw()
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
