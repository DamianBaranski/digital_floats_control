import base64
from typing import Generic, TypeVar, Union, Optional
from widgets.user_settings import UserSettings

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
            frame = bytes([in_data.len]) + data_bytes + in_data.crc.to_bytes(2, 'big')
            frame =  base64.b64encode(bytes([ord(in_data.cmd)])+frame)+b'\r'
            return frame
        except Exception as e:
            print(f"Encoding error: {e}")
            return ""

    def decode_response(self, instr: str) -> Optional[Union[DataOutType]]:
        try:
            print("Received:", instr)
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
        self.logs = ""

    def getVersion(self):
        if not self.uart.isOpen():
            return "N/A"
        
        cmd_str = self.protocol.InData(cmd='v')
        encoded_cmd = self.protocol.encode_output(cmd_str)
        self.uart.send(encoded_cmd)
        response = self.uart.read()
        decoded_response = self.protocol.decode_response(response)
        return decoded_response.decode('utf-8').rstrip('\x00')

    def getLogs(self):
        return self.uart.getLogs()
    
    def scanI2c(self, address):
        if not self.uart.isOpen():
            return False
        
        cmd_str = self.protocol.InData(cmd='s', data=address.to_bytes())
        encoded_cmd = self.protocol.encode_output(cmd_str)
        self.uart.send(encoded_cmd)
        response = self.uart.read()
        decoded_response = self.protocol.decode_response(response)
        return decoded_response
    
    def getUserSettings(self):
        settings = UserSettings()
        cmd_str = self.protocol.InData(cmd='u')
        encoded_cmd = self.protocol.encode_output(cmd_str)
        self.uart.send(encoded_cmd)
        response = self.uart.read()
        data = self.protocol.decode_response(response)
        settings.fromByteArray(data)
        return settings
        
    def updateUserSettings(self, settings):
        cmd_str = self.protocol.InData(cmd='U', data=settings.toByteArray())
        encoded_cmd = self.protocol.encode_output(cmd_str)
        self.uart.send(encoded_cmd)
        response = self.uart.read()
        return self.protocol.decode_response(response)
        
