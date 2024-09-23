from serial.tools.list_ports import comports
import serial

class ComPort:
    def __init__(self):
        self.serial = serial.Serial()

    def open(self, port):
        self.serial.close()
        self.serial = serial.Serial(port=port, baudrate=115200, timeout=1.0)

    def close(self):
        return self.serial.close()
    
    def isOpen(self):
        return self.serial.is_open
    
    def getPortsList(self):
        return [port.device for port in comports()]

    def send(self, data):
        self.serial.write(data)
        return True
    
    def read(self):
        while True:
            result = self.serial.readline().decode().strip()
            if not 'LOG:' in result:
                return result 