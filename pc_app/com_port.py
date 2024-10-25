from serial.tools.list_ports import comports
import serial
import queue
import threading
import time
import logging

# Configure logging for debugging
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")
logger = logging.getLogger(__name__)

class ComPort:
    """
    Manages serial communication over a specified COM port with support
    for sending, receiving, and logging data, as well as threaded communication.
    """
    def __init__(self, onconnected=None):
        """
        Initializes the ComPort instance and starts the communication thread.
        """
        self.serial = serial.Serial()
        self.onconnected = onconnected  # Callback function for connection state changes
        self.queue = queue.Queue()      # Queue for send/receive tasks
        self.logs = queue.Queue()       # Queue for storing logs
        self.timeout = 30               # Timeout for send/receive operations
        self.stop_event = threading.Event()  # Event to stop the thread
        self.thread = threading.Thread(target=self._thread_fnc)
        self.thread.start()
        logger.info("ComPort initialized and thread started.")

    def open(self, port, timeout=2):
        """
        Opens a serial connection to the specified port with defined settings.
        Calls onconnected if set, to indicate the connection status.
        """
        self.serial.close()  # Close any open serial connection
        self.serial = serial.Serial(port=port, baudrate=115200, timeout=timeout)
        if self.isOpen() and self.onconnected:
            self.onconnected(True)
            logger.info(f"Opened serial port: {port}")

    def close(self):
        """
        Closes the serial connection and calls onconnected callback if set.
        """
        self.serial.close()
        if not self.isOpen() and self.onconnected:
            self.onconnected(False)
            logger.info("Serial port closed.")

    def isOpen(self):
        """
        Checks if the serial port is open.
        """
        return self.serial.is_open

    def getPortsList(self):
        """
        Returns a list of available serial ports.
        """
        ports = [port.device for port in comports()]
        return ports

    def send(self, data):
        """
        Sends data through the serial port.
        """
        try:
            logger.debug(f"Sending data: {data}")
            self.serial.write(data)
            return True
        except Exception as e:
            logger.error(f"Failed to send data: {e}")
            return False

    def read(self):
        """
        Reads data from the serial port.
        Handles logs separately if 'LOG:' is found in the output.
        """
        try:
            while True:
                result = self.serial.readline().decode().strip()
                if 'LOG:' in result:
                    self._addLogs(result)  # Store log separately
                else:
                    return result
        except Exception as e:
            logger.error(f"Failed to read data: {e}")
            return ""

    def _read_logs(self):
        """
        Reads available logs from the serial port and stores them.
        """
        try:
            while self.isOpen() and self.serial.in_waiting > 0:
                result = self.serial.readline().decode().strip()
                if 'LOG:' in result:
                    self._addLogs(result)
        except Exception as e:
            logger.error(f"Error reading logs: {e}")
        
    def getLogs(self):
        """
        Retrieves all logs accumulated in the log queue.
        """
        self._read_logs()
        logs = ""
        while not self.logs.empty():
            logs += self.logs.get() + '\n'
        logger.info("Retrieved logs.")
        return logs

    def _addLogs(self, log):
        """
        Adds a log message to the logs queue.
        """
        self.logs.put(log)
        logger.debug(f"Log added: {log}")
        
    def send_receive(self, data, fnc):
        """
        Queues a send/receive operation with data and callback function fnc.
        Limits the queue size to prevent excessive requests.
        """
        if self.queue.qsize() < 20:
            self.queue.put([data, fnc, time.time()])
            logger.debug("Queued send/receive operation.")
        else:
            logger.warning("Queue limit reached; send/receive operation not added.")

    def _thread_fnc(self):
        """
        Thread function to handle queued send/receive tasks in a loop.
        Checks connection status, timeouts, and queue size to manage communication.
        """
        while not self.stop_event.is_set():  # Continue until stop_event is set
            try:
                [data, fnc, t] = self.queue.get(timeout=1)  # Timeout to avoid indefinite blocking
                if not self.isOpen():
                    fnc("")
                    logger.warning("Serial port not open; skipping send/receive operation.")
                    continue

                # Timeout or excessive queue size handling
                if t + self.timeout < time.time():
                    fnc("")
                    logger.warning("Operation timed out.")
                    continue

                if self.queue.qsize() > 20:
                    fnc("")
                    logger.warning("Queue size exceeded limit; skipping operation.")
                    continue

                # Send data and read the response
                self.send(data)
                ret = self.read()
                fnc(ret)
            except queue.Empty:
                continue  # No item in queue; ignore and retry
            #except Exception as e:
            #    logger.error(f"Thread error: {e}")

    def stop(self):
        """
        Signals the thread to stop and waits for it to terminate.
        """
        self.stop_event.set()  # Signal to stop the thread
        self.thread.join()     # Wait for thread to finish
        logger.info("ComPort thread stopped.")
