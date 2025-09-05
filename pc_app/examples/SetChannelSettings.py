import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests
from core.datatypes.channel_settings import ChannelSettings

class Example:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.connect(port)

    def set_channel_settings(self, channel, rudder):
        settings = ChannelSettings()
        settings.setChannel(channel)
        settings.setEnable(True)
        settings.setBridge(channel == 0)
        settings.setInverseMotor(False)
        settings.setInverseUpLimitSwitch(False)
        settings.setInverseDownLimitSwitch(False)
        settings.setInverseLimitSwitch(False)
        settings.setRudder(rudder)
        settings.setTimeout(5)  # seconds
        settings.setBridgeChannel(0)
        settings.setMaxCurrentWarningLimit(0.5)  # A units
        settings.setMaxCurrentErrorLimit(1)    # A units
        settings.setMinCurrentLimit(0)          # A units
        print(settings)
        self.device_client.command(requests.UpdateChannelSettingsRequest(settings), self.channel_settings_update)
        time.sleep(1)
        
    def channel_settings_update(self, settings):
        print(settings)

if __name__ == "__main__":
    app = Example("/dev/ttyUSB0")
    for i in range(2):
        app.set_channel_settings(i, rudder=True)
        
    for i in range(2, 6):
        app.set_channel_settings(i, rudder=False)
