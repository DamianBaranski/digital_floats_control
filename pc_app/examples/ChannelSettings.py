import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests

class Example:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.command(requests.GetChannelSettingsRequest(0), self.channel_settings_update)
        self.device_client.connect(port)

    def channel_settings_update(self, settings):
        print(settings)

if __name__ == "__main__":
    app = Example("/dev/ttyUSB0")
    time.sleep(0.1)
