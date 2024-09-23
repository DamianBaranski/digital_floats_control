
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

if __name__ == "__main__":
    # Initialize UART connection
    uart = UARTConnection(port='/dev/ttyUSB0', baudrate=115200)
    
    # Initialize Protocol handler
    protocol = Protocol[Union[bytes, int], Union[bytes, int]]()

    try:
        # Create input data
        input_data = b""
        in_data = protocol.InData(cmd='v', data=input_data)

        # Encode the data
        encoded_cmd = protocol.encode_output(in_data)

        # Send command via UART
        uart.send(encoded_cmd)

        # Receive response via UART
        response = uart.receive()

        # Decode the response
        decoded_response = protocol.decode_response(response)
        print(f"Response: {decoded_response}")

    finally:
        # Close UART connection
        uart.close()