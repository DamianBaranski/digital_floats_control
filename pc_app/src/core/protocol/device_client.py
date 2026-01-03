import serial
import serial.tools.list_ports
import threading
import time
import logging
import glob
import os
from typing import Callable, List, Tuple
from core.protocol.requests import BaseProtocolMessage

# SIM_ENABLED: Set to 1 to enable simulator port detection, 0 to disable
SIM_ENABLED = 1

logger = logging.getLogger(__name__)

class DeviceClient:
    def __init__(self):
        self.serial = None
        self.logs: List[str] = []
        self.subscriptions: List[Tuple[BaseProtocolMessage, Callable]] = []
        self.commands: List[Tuple[BaseProtocolMessage, Callable]] = []
        self._stop_flag = threading.Event()
        self.thread = None

    def getPortList(self):
        # Get regular serial ports
        ports = serial.tools.list_ports.comports()
        port_list = [
            port.device
            for port in ports
            if port.description.strip().lower() != 'n/a'
        ]
        
        # Add simulator ports (pts devices) if enabled
        if SIM_ENABLED:
            sim_ports = self._get_simulator_ports()
            port_list.extend(sim_ports)
        
        return port_list
    
    def _get_simulator_ports(self) -> List[str]:
        """Find available simulator ports (pseudo-terminals)."""
        sim_ports = []
        
        # Look for pts devices that could be simulators
        pts_pattern = '/dev/pts/*'
        for pts_path in glob.glob(pts_pattern):
            try:
                # Skip ptmx and non-numeric pts
                basename = os.path.basename(pts_path)
                if basename == 'ptmx' or not basename.isdigit():
                    continue
                    
                # Check if it's accessible (read/write)
                if os.access(pts_path, os.R_OK | os.W_OK):
                    sim_ports.append(f"sim:{pts_path}")
            except (OSError, ValueError):
                continue
        
        return sim_ports
        
    def subscribe(self, message: BaseProtocolMessage, callback: Callable):
        self.subscriptions.append((message, callback))

    def command(self, message: BaseProtocolMessage, callback: Callable):
        self.commands.append((message, callback))

    def _add_logs(self, log_line: str):
        self.logs.append(log_line)
        logger.info(f"Device log: {log_line}")

    def connect(self, port: str, baudrate: int = 115200, timeout: float = 0.1):
        if self.serial and self.serial.is_open:
            self.disconnect()

        self._stop_flag.clear()
        
        # Handle simulator port prefix (only if SIM_ENABLED)
        actual_port = port
        if SIM_ENABLED and port.startswith('sim:'):
            actual_port = port[4:]  # Strip 'sim:' prefix
            logger.info(f"Connecting to simulator port: {actual_port}")
        
        self.serial = serial.Serial(actual_port, baudrate=baudrate, timeout=timeout)

        if self.serial.is_open:
            self.thread = threading.Thread(target=self._thread_fn, daemon=True)  # recreate thread
            self.thread.start()
            return True
        else:
            return False
    
    def disconnect(self):
        self._stop_flag.set()
        if self.thread is not None and self.thread.is_alive():
            self.thread.join()
        if self.serial is not None:
            self.serial.close()
    
    def isConnected(self):
        if self.serial is not None:
            return self.serial.is_open
        else:
            return False
        
    def _thread_fn(self):
        while not self._stop_flag.is_set():
            start = time.time()

            # Handle subscriptions
            for message, callback in list(self.subscriptions):
                self._handle_message(message, callback)

            # Handle one-time commands
            for message, callback in list(self.commands):
                self._handle_message(message, callback)
            self.commands.clear()

            elapsed = time.time() - start
            logger.debug(f"Loop time: {elapsed:.3f}s")
            time.sleep(0.1)

    def _handle_message(self, message: BaseProtocolMessage, callback: Callable):
        try:
            msg = message.send_request()
            self.serial.write(msg)
            self._await_response(message, callback)
        except Exception as e:
            logger.warning(f"Error during message handling: {e}")

    def _await_response(self, message: BaseProtocolMessage, callback: Callable):
        start_time = time.time()
        timeout = 2.0

        while time.time() - start_time < timeout:
            if self.serial.in_waiting:
                response = self.serial.readline()
                decoded = response.decode(errors="ignore").strip()

                if 'LOG:' in decoded:
                    self._add_logs(decoded)
                    continue

                try:
                    parsed = message.parse_response(response)
                    if parsed is not None:
                        callback(parsed)
                    return
                except Exception as e:
                    logger.warning(f"Failed to parse response: {e}")
                    return
            else:
                time.sleep(0.01)

        logger.warning("Timeout waiting for response.")
