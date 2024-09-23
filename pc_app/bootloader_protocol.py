
import serial
from protocol import Protocol
from typing import Union

class UARTConnection:
    def __init__(self, port: str, baudrate: int, timeout: float = 1.0):
        self.ser = serial.Serial(port, baudrate, timeout=timeout)
        if not self.ser.is_open:
            self.ser.open()

    def send(self, data: str):
        data = data+'\r'
        self.ser.write(data.encode())

    def receive(self) -> str:
        while True:
            result = self.ser.readline().decode().strip()
            if not 'LOG:' in result:
                return result 

    def close(self):
        if self.ser.is_open:
            self.ser.close()


class BootloaderProtocol:
    def __init__(self, uart):
        self.protocol = Protocol[Union[bytes, int], Union[bytes, int]]()
        self.uart = uart

    def upgrade(self, file_data):
        self._wait_for_bootloader()
        self._send_file_size(file_data)


    def _send_file_size(self, file_data):
        cmd_str = self.protocol.InData(cmd='l', data = len(file_data).to_bytes(2, 'big'))
        encoded_cmd = self.protocol.encode_output(cmd_str)
        print(f"encoded:{encoded_cmd}")
        self.uart.send(encoded_cmd)
        response = self.uart.receive()
        print(f"response:{response}")
        decoded_response = self.protocol.decode_response(response)
        return True

    def _wait_for_bootloader(self):
        print('Waiting for bootloader...')
        while True:
            line = self.uart.receive()
            if "Bootloader BS" in line:
                print("Bootloader BS detected, ready to upload.")
                break


if __name__ == "__main__":
    # Initialize UART connection
    uart = UARTConnection(port='/dev/ttyUSB0', baudrate=115200)
    
    booltoader = BootloaderProtocol(uart)
    booltoader.upgrade("asdasdasdnkasdasldnasdlaksndasldkasndalskdnasdl")
    