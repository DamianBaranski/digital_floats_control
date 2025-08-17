import logging
import base64
from typing import Generic, TypeVar, Union, Optional
from core.datatypes.monitoring_data import MonitoringData
from core.datatypes.status_data import StatusData
from core.datatypes.firmware_info import FirmwareInfo
from core.datatypes.channel_settings import ChannelSettings
from core.datatypes.remote_control_data import RemoteControlData

# Configure logging with INFO level and log format
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")
logger = logging.getLogger(__name__)

# Define generic types for protocol input and output data
DataInType = TypeVar('DataInType')
DataOutType = TypeVar('DataOutType')

class Protocol(Generic[DataInType, DataOutType]):
    class InData:
        def __init__(self, cmd: str, data: Union[DataInType, bytes] = bytes(), crc: int = 0):
            self.cmd = cmd
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1
            self.data = data
            self.crc = crc

    class OutData:
        def __init__(self, cmd: str, data: Union[DataOutType, bytes], crc: int = 0):
            self.cmd = cmd
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1
            self.data = data
            self.crc = crc

    @staticmethod
    def encode_output(in_data: 'Protocol.InData') -> bytes:
        try:
            data_bytes = bytes(in_data.data)
            frame = bytes([in_data.len]) + data_bytes + in_data.crc.to_bytes(2, 'big')
            frame = base64.b64encode(bytes([ord(in_data.cmd)]) + frame) + b'\r'
            logger.debug(f"Encoded frame: {frame}")
            return frame
        except Exception as e:
            logger.error(f"Encoding error: {e}")
            return b""

    @staticmethod
    def decode_response(instr: str) -> Optional[Union[DataOutType, bytes]]:
        try:
            logger.debug(f"Received response: {instr}")
            decoded_bytes = base64.b64decode(instr)
            cmd = chr(decoded_bytes[0])
            data_len = decoded_bytes[1]
            data = decoded_bytes[2:-2]
            crc = int.from_bytes(decoded_bytes[-2:], 'big')
            out_data = Protocol.OutData(cmd, data, crc)
            logger.debug(f"Decoded data: cmd={cmd}, data_len={data_len}, crc={crc}")
            return out_data.data
        except Exception as e:
            logger.error(f"Decoding error: {e}")
            return None

    @staticmethod
    def calculate_crc(data: bytes) -> int:
        return 0

class BaseProtocolMessage:
    def send_request(self):
        raise NotImplementedError

    def parse_response(self, response_bytes):
        raise NotImplementedError


class FirmwareInfoRequest(BaseProtocolMessage):
    def send_request(self):
        cmd_str = Protocol.InData(cmd='v')
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            data = FirmwareInfo()
            data.fromByteArray(decoded)
            return data
        return None


class MonitoringChannelRequest(BaseProtocolMessage):
    def __init__(self, channel):
        self.channel = channel

    def send_request(self):
        cmd_str = Protocol.InData(cmd='m', data=self.channel.to_bytes(1, 'little'))
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            data = MonitoringData()
            data.fromByteArray(decoded)
            return data
        return None


class RemoteControlRequest(BaseProtocolMessage):
    def __init__(self, data: RemoteControlData):
        self.data = data
        
    def send_request(self):
        data = self.data.toByteArray()
        cmd_str = Protocol.InData(cmd='l', data=data)
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        return bool.from_bytes(decoded, 'little') if decoded else False


class I2CBusScanRequest(BaseProtocolMessage):
    def __init__(self, address):
        self.address = address

    def send_request(self):
        cmd_str = Protocol.InData(cmd='s', data=self.address.to_bytes(1, 'little'))
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        return bool.from_bytes(decoded, 'little') if decoded else False

    
class ResetDeviceRequest(BaseProtocolMessage):
    def send_request(self):
        cmd_str = Protocol.InData(cmd='r')
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        # We assume success if any response is returned
        decoded = Protocol.decode_response(response_bytes)
        return decoded is not None


class UploadFirmwareRequest(BaseProtocolMessage):
    def __init__(self, data: bytes, ptr: int, length: int):
        self.data = data
        self.ptr = ptr
        self.length = length

    def send_request(self):
        data = (
            self.ptr.to_bytes(2, 'little') +
            self.length.to_bytes(2, 'little') +
            self.data
        )
        cmd_str = Protocol.InData(cmd='u', data=data)
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        return bool.from_bytes(decoded, 'little') if decoded else False
    
    
class GetChannelSettingsRequest(BaseProtocolMessage):
    def __init__(self, channel):
        self.channel = channel

    def send_request(self):
        cmd_str = Protocol.InData(cmd='c', data=self.channel.to_bytes(1, 'little'))
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            data = ChannelSettings()
            data.fromByteArray(decoded)
            return data
        return None


class UpdateChannelSettingsRequest(BaseProtocolMessage):
    def __init__(self, channel_settings: ChannelSettings):
        self.settings = channel_settings

    def send_request(self):
        data = self.settings.toByteArray()
        cmd_str = Protocol.InData(cmd='C', data=data)
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        return decoded  # Could be interpreted further if needed

class StatusRequest(BaseProtocolMessage):
    def __init__(self):
        self.status = StatusData()

    def send_request(self):
        cmd_str = Protocol.InData(cmd='S')  # 'S' for status
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            self.status.fromByteArray(decoded)
            return self.status
        return None