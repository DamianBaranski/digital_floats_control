from serial.tools.list_ports import comports

class ComPort:
    def __init__(self):
        pass

    def getPortsList(self):
        return comports()
