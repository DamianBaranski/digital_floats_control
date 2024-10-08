from serial.tools.list_ports import comports
import serial
import queue
import threading
import time

class ComPort:
    def __init__(self, onconnected=None):
        self.serial = serial.Serial()
        self.onconnected = onconnected
        self.queue = queue.Queue()
        self.logs = queue.Queue()
        self.timeout = 30
        self.stop_event = threading.Event()  # Stop event for the thread
        self.thread = threading.Thread(target=self._thread_fnc)
        self.thread.start()
        
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
            self.serial.write(data)
            return True
        except:
            return False
    
    def read(self):
        try:
            while True:
                result = self.serial.readline().decode().strip()
                if 'LOG:' in result:
                    self._addLogs(result)
                else:
                    return result
        except:
            return ""

    def _read_logs(self):
        while self.isOpen() and self.serial.in_waiting>0:
            result = self.serial.readline().decode().strip()
            if 'LOG:' in result:
                self._addLogs(result)

    def getLogs(self):
        self._read_logs()        
        logs = ""
        while not self.logs.empty():
            logs = logs + self.logs.get() + '\n'
        return logs

    def _addLogs(self, log):
        self.logs.put(log)
        
    def send_receive(self, data, fnc):
        if self.queue.qsize()<20:
            self.queue.put([data, fnc, time.time()])

    def _thread_fnc(self):
        while not self.stop_event.is_set():  # Check if the stop event is set
            try:
                [data, fnc, t] = self.queue.get(timeout=1)  # Use timeout to prevent blocking indefinitely
                if not self.isOpen():
                    fnc("")
                    continue

                if t + self.timeout < time.time():
                    fnc("")
                    continue

                if self.queue.qsize() > 20:
                    fnc("")
                    continue

                self.send(data)
                ret = self.read()
                fnc(ret)
            except queue.Empty:
                continue  # Ignore timeout for queue.get
            except Exception as e:
                print(f"Thread error: {e}")

    def stop(self):
        self.stop_event.set()  # Signal the thread to stop
        self.thread.join()      # Wait for the thread to finish