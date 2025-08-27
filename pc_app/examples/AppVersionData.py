import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests

class Example:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.command(requests.FirmwareInfoRequest(), self.firmware_info_update)
        self.device_client.connect(port)

    def firmware_info_update(self, version):
        print(version)

if __name__ == "__main__":
    app = Example("/dev/ttyUSB0")
    time.sleep(0.1)
