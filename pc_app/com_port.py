from serial.tools.list_ports import comports
import serial

class ComPort:
    def __init__(self, onconnected=None):
        self.serial = serial.Serial()
        self.logs = ""
        self.onconnected = onconnected

    def open(self, port):
        self.serial.close()
        self.serial = serial.Serial(port=port, baudrate=115200, timeout=1)
        if self.isOpen() and self.onconnected!=None:
            self.onconnected(True)

    def close(self):
        self.serial.close()
        if (not self.isOpen()) and self.onconnected!=None:
            self.onconnected(False)            
    
    def isOpen(self):
        return self.serial.is_open
    
    def getPortsList(self):
        return [port.device for port in comports()]

    def send(self, data):
        try:
            print("write", data)
            self.getLogs()
            self.serial.write(data)
            return True
        except:
            return False
    
    def read(self):
        while True:
            result = self.serial.readline().decode().strip()
            if 'LOG:' in result:
                self._addLogs(result)
            else:
                return result

    def getLogs(self):
        while self.isOpen() and self.serial.inWaiting():
            result = self.serial.readline().decode().strip()
            if 'LOG:' in result:
                self._addLogs(result)

        logs = self.logs
        self.logs = ''
        return logs

    def _addLogs(self, log):
        self.logs = self.logs + log + '\n'

        