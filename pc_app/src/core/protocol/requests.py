import logging
import base64
from typing import Generic, TypeVar, Union, Optional
from core.datatypes.monitoring_data import MonitoringData
from core.datatypes.status_data import StatusData

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


class FirmwareVersionRequest(BaseProtocolMessage):
    def send_request(self):
        cmd_str = Protocol.InData(cmd='v')
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        if len(response_bytes) == 0:
            return "N/A"
        decoded = Protocol.decode_response(response_bytes)
        return decoded.decode('utf-8').rstrip('\x00') if decoded else "N/A"


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


class SimulateControlRequest(BaseProtocolMessage):
    def __init__(self, ldg_gear_switch_state, rudder_switch_state, test_button_state):
        self.ldg_gear_switch_state = ldg_gear_switch_state
        self.rudder_switch_state = rudder_switch_state
        self.test_button_state = test_button_state

    def send_request(self):
        data = (
            self.test_button_state.to_bytes(1, 'little') +
            self.ldg_gear_switch_state.to_bytes(1, 'little') +
            self.rudder_switch_state.to_bytes(1, 'little')
        )
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


class TestRelaysRequest(BaseProtocolMessage):
    def __init__(self, pcf_addr, pcf_channel, ina_addr):
        self.pcf_addr = pcf_addr
        self.pcf_channel = pcf_channel
        self.ina_addr = ina_addr

    def send_request(self):
        data = (
            self.ina_addr.to_bytes(1, 'little') +
            self.pcf_addr.to_bytes(1, 'little') +
            self.pcf_channel.to_bytes(1, 'little')
        )
        cmd_str = Protocol.InData(cmd='t', data=data)
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
    
class GetUserSettingsRequest(BaseProtocolMessage):
    def __init__(self, settings_obj):
        self.settings = settings_obj  # Must be an instance of UserSettingsPanel

    def send_request(self):
        cmd_str = Protocol.InData(cmd='u')
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            self.settings.fromByteArray(decoded)
        return self.settings


class UpdateUserSettingsRequest(BaseProtocolMessage):
    def __init__(self, settings_obj):
        self.settings = settings_obj  # Must be an instance of UserSettingsPanel

    def send_request(self):
        cmd_str = Protocol.InData(cmd='U', data=self.settings.toByteArray())
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        return decoded  # Return as-is; calling code may decide how to interpret this


class GetChannelSettingsRequest(BaseProtocolMessage):
    def __init__(self, channel, settings_obj):
        self.channel = channel
        self.settings = settings_obj  # Must be an instance of ChannelSettings

    def send_request(self):
        cmd_str = Protocol.InData(cmd='c', data=self.channel.to_bytes(1, 'little'))
        return Protocol.encode_output(cmd_str)

    def parse_response(self, response_bytes):
        decoded = Protocol.decode_response(response_bytes)
        if decoded:
            self.settings.fromByteArray(decoded)
        return self.settings


class UpdateChannelSettingsRequest(BaseProtocolMessage):
    def __init__(self, channel, settings_obj):
        self.channel = channel
        self.settings = settings_obj  # Must be an instance of ChannelSettings

    def send_request(self):
        data = self.settings.toByteArray() + self.channel.to_bytes(1, 'little')
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