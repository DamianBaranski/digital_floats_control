import base64
from typing import Generic, TypeVar, Union, Optional

# Generic types for input and output data
DataInType = TypeVar('DataInType')
DataOutType = TypeVar('DataOutType')

class Protocol(Generic[DataInType, DataOutType]):
    class InData:
        def __init__(self, cmd: str, data: Union[DataInType] = bytes(), crc: int = 0):
            self.cmd = cmd
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1
            self.data = data
            self.crc = crc

    class OutData:
        def __init__(self, cmd: str, data: Union[DataOutType], crc: int = 0):
            self.cmd = cmd
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1
            self.data = data
            self.crc = crc

    def encode_output(self, in_data: 'Protocol.InData') -> bytes:
        try:
            data_bytes = bytes(in_data.data)
            frame = bytes([ord(in_data.cmd)]) + bytes([in_data.len]) + data_bytes + in_data.crc.to_bytes(2, 'big')
            return base64.b64encode(frame)
        except Exception as e:
            print(f"Encoding error: {e}")
            return ""

    def decode_response(self, instr: str) -> Optional[Union[DataOutType]]:
        try:
            decoded_bytes = base64.b64decode(instr)
            print(decoded_bytes)
            cmd = chr(decoded_bytes[0])
            data_len = decoded_bytes[1]
            data = decoded_bytes[2:-2]  # excluding cmd, len, and CRC
            crc = int.from_bytes(decoded_bytes[-2:], 'big')

            out_data = self.OutData(cmd, data, crc)
            return out_data.data
        except Exception as e:
            print(f"Decoding error: {e}")
            return None

    def calculate_crc(self, data: bytes) -> int:
        # Placeholder for CRC calculation, can be implemented based on your specific CRC algorithm
        return 0

class AppProtocol:
    def __init__(self, comport):
        self.protocol = Protocol[Union[bytes, int], Union[bytes, int]]()
        self.uart = comport

    def getVersion(self):
        if not self.uart.isOpen():
            return "N/A"
        
        cmd_str = self.protocol.InData(cmd='v')
        encoded_cmd = self.protocol.encode_output(cmd_str)
        print(f"encoded:{encoded_cmd}")
        self.uart.send(encoded_cmd)
        response = self.uart.read()
        print(f"response:{response}")
        decoded_response = self.protocol.decode_response(response)
        return str(decoded_response)