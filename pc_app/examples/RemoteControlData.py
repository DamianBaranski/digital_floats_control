import time
from core.protocol.device_client import DeviceClient
from core.protocol import requests
from core.datatypes.remote_control_data import RemoteControlData

class Example:
    def __init__(self, port):
        self.device_client = DeviceClient()
        self.device_client.subscribe(requests.StatusDataRequest(), self.status_update)
        self.device_client.connect(port)

    def status_update(self, data):
        print(data)
        
    def change_remote_control(self, ldg):
        data = RemoteControlData(ldg, False, False)
        self.device_client.command(requests.RemoteControlRequest(data), self.remote_control_update)
        
    def remote_control_update(self, data):
        pass

if __name__ == "__main__":
    app = Example("/dev/ttyUSB0")
    while True:
        app.change_remote_control(True)
        time.sleep(0.5)
        app.change_remote_control(False)
        time.sleep(0.5)