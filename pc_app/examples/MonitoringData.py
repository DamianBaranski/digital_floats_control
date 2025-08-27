import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests

class Example:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.command(requests.MonitoringDataRequest(0), self.monitoring_data_update)
        self.device_client.connect(port)

    def monitoring_data_update(self, data):
        print(data)

if __name__ == "__main__":
    app = Example("/dev/ttyUSB0")
    time.sleep(0.1)
