"""
Protocol encoder/decoder for Device Simulator.
Matches the protocol used by the PC app.
"""
import base64
import struct
from typing import Optional, Tuple, Dict


class Protocol:
    """Protocol handler matching PC app's Protocol class."""
    
    # Command characters
    CMD_STATUS = 'S'
    CMD_FIRMWARE_INFO = 'v'
    CMD_MONITORING = 'm'
    CMD_REMOTE_CONTROL = 'l'
    CMD_CHANNEL_SETTINGS = 'c'
    CMD_UPDATE_CHANNEL_SETTINGS = 'C'
    CMD_ERROR_STATUS = 'e'
    CMD_I2C_SCAN = 's'
    CMD_RESET = 'r'
    CMD_UPLOAD_FIRMWARE = 'u'
    
    @staticmethod
    def decode_request(raw_data: bytes) -> Optional[Tuple[str, bytes]]:
        """
        Decode incoming request from PC app.
        
        Args:
            raw_data: Raw bytes received (base64 encoded + CR)
            
        Returns:
            Tuple of (command_char, data_bytes) or None if decode fails
        """
        try:
            # Strip CR/LF if present
            data = raw_data.strip()
            if not data:
                return None
            
            print(f"DEBUG decode: raw_data={raw_data}, stripped={data}")
                
            # Decode base64
            decoded = base64.b64decode(data)
            print(f"DEBUG decode: decoded={decoded.hex()}, len={len(decoded)}")
            
            # Extract fields: cmd (1) + len (1) + crc (2) + data
            if len(decoded) < 4:
                print(f"DEBUG decode: Too short, need at least 4 bytes, got {len(decoded)}")
                return None
                
            cmd = chr(decoded[0])
            data_len = decoded[1]
            # crc = int.from_bytes(decoded[2:4], 'big')  # Skip CRC for now
            payload = decoded[4:4 + data_len] if data_len > 0 else b''
            
            print(f"DEBUG decode: cmd={cmd}, data_len={data_len}, payload={payload.hex()}")
            return (cmd, payload)
            
        except Exception as e:
            print(f"Protocol decode error: {e}")
            import traceback
            traceback.print_exc()
            return None
    
    @staticmethod
    def encode_response(cmd: str, data: bytes = b'', crc: int = 0) -> bytes:
        """
        Encode response to send to PC app.
        
        Args:
            cmd: Command character
            data: Response data bytes
            crc: CRC value (default 0)
            
        Returns:
            Base64 encoded frame with CR terminator
        """
        try:
            data_len = len(data)
            frame = bytes([ord(cmd), data_len]) + crc.to_bytes(2, 'big') + data
            encoded = base64.b64encode(frame) + b'\r'
            return encoded
            
        except Exception as e:
            print(f"Protocol encode error: {e}")
            return b''


class StatusDataEncoder:
    """Encodes StatusData for protocol transmission."""
    
    @staticmethod
    def encode(power_voltage: float, memory_usage: int, uptime: int,
               ldg_gear_switch: bool, rudder_switch: bool,
               test_button: bool, remote_control_status: bool) -> bytes:
        """
        Encode StatusData to bytes.
        
        Format: <hhIB
        - power_voltage: short (value * 10)
        - memory_usage: short
        - uptime: unsigned int
        - switches: byte (bit flags)
        """
        # Convert voltage to fixed point (x10)
        voltage_int = int(power_voltage * 10)
        
        # Pack switches into byte
        switches = (
            (1 if ldg_gear_switch else 0) |
            ((1 if rudder_switch else 0) << 1) |
            ((1 if test_button else 0) << 2) |
            ((1 if remote_control_status else 0) << 3)
        )
        
        return struct.pack('<hhIB', voltage_int, memory_usage, uptime, switches)


class FirmwareInfoEncoder:
    """Encodes FirmwareInfo for protocol transmission."""
    
    @staticmethod
    def encode(app_version: str, hardware_version: str, serial_number: str,
               build_date: str, build_time: str, git_commit: str) -> bytes:
        """
        Encode FirmwareInfo to bytes.
        
        Format: <20s20s20s20s20s40s
        """
        fmt = '<20s20s20s20s20s40s'
        return struct.pack(
            fmt,
            app_version.encode('utf-8')[:20].ljust(20, b'\x00'),
            hardware_version.encode('utf-8')[:20].ljust(20, b'\x00'),
            serial_number.encode('utf-8')[:20].ljust(20, b'\x00'),
            build_date.encode('utf-8')[:20].ljust(20, b'\x00'),
            build_time.encode('utf-8')[:20].ljust(20, b'\x00'),
            git_commit.encode('utf-8')[:40].ljust(40, b'\x00'),
        )


class MonitoringDataEncoder:
    """Encodes MonitoringData for protocol transmission."""
    
    STATE_UP = 0
    STATE_DOWN = 1
    STATE_MOVING = 2
    STATE_ERROR = 3
    
    @staticmethod
    def encode(timestamp: int, current_ma: int, channel: int, state: int,
               up_switch: bool, down_switch: bool) -> bytes:
        """
        Encode MonitoringData to bytes.
        
        Format: <IhBBB
        - timestamp: unsigned int (ms)
        - current: short (mA)
        - channel: byte
        - state: byte
        - switches: byte (bit flags)
        """
        switches = (1 if up_switch else 0) | ((1 if down_switch else 0) << 1)
        return struct.pack('<IhBBB', timestamp, current_ma, channel, state, switches)


class ChannelSettingsEncoder:
    """Encodes ChannelSettings for protocol transmission."""
    
    @staticmethod
    def encode(channel: int, enable: bool, bridge: bool, inverse_motor: bool,
               inverse_up_limit: bool, inverse_down_limit: bool,
               inverse_limit: bool, rudder: bool, timeout: int,
               bridge_channel: int, max_current_warning: float,
               max_current_error: float, min_current: float) -> bytes:
        """
        Encode ChannelSettings to bytes.
        
        Format: <BBBBHHH
        """
        bit_fields = (
            (1 if enable else 0) |
            ((1 if bridge else 0) << 1) |
            ((1 if inverse_motor else 0) << 2) |
            ((1 if inverse_up_limit else 0) << 3) |
            ((1 if inverse_down_limit else 0) << 4) |
            ((1 if inverse_limit else 0) << 5) |
            ((1 if rudder else 0) << 6)
        )
        
        return struct.pack(
            '<BBBBHHH',
            channel,
            bit_fields,
            bridge_channel,
            timeout,
            int(max_current_warning * 1000),
            int(max_current_error * 1000),
            int(min_current * 1000)
        )


class ErrorStatusEncoder:
    """Encodes ErrorStatus for protocol transmission."""
    
    @staticmethod
    def encode(channel_errors: list, channel_warnings: list, 
               system_warnings: int, num_channels: int = 6) -> bytes:
        """
        Encode ErrorStatus to bytes.
        
        Format: <13B (6 errors + 6 warnings + 1 system)
        
        Args:
            channel_errors: List of 6 error bytes
            channel_warnings: List of 6 warning bytes
            system_warnings: System warning byte
        """
        data = bytes(channel_errors[:num_channels]) + \
               bytes(channel_warnings[:num_channels]) + \
               bytes([system_warnings])
        return data


class RemoteControlDataDecoder:
    """Decodes RemoteControlData from received bytes."""
    
    @staticmethod
    def decode(data: bytes) -> Tuple[bool, bool, bool]:
        """
        Decode RemoteControlData from bytes.
        
        Returns: (ldg_gear, rudder, test_button)
        """
        if not data:
            return (False, False, False)
        
        bit_fields = data[0]
        ldg_gear = bool(bit_fields & 0x01)
        rudder = bool(bit_fields & 0x02)
        test_button = bool(bit_fields & 0x04)
        
        return (ldg_gear, rudder, test_button)


class ChannelSettingsDecoder:
    """Decodes ChannelSettings from received bytes."""
    
    @staticmethod
    def decode(data: bytes) -> Optional[Dict]:
        """
        Decode ChannelSettings from bytes.
        
        Format: <BBBBHHH
        - channel: byte
        - bit_fields: byte
        - bridge_channel: byte
        - timeout: byte
        - max_current_warning: short (mA)
        - max_current_error: short (mA)
        - min_current: short (mA)
        
        Returns: Dictionary with all channel settings or None if decode fails
        """
        if len(data) < 10:
            return None
        
        try:
            unpacked = struct.unpack('<BBBBHHH', data[:10])
            
            channel = unpacked[0]
            bit_fields = unpacked[1]
            bridge_channel = unpacked[2]
            timeout = unpacked[3]
            max_current_warning = unpacked[4] * 0.001  # Convert from mA to A
            max_current_error = unpacked[5] * 0.001
            min_current = unpacked[6] * 0.001
            
            return {
                'channel': channel,
                'enable': bool(bit_fields & (1 << 0)),
                'bridge': bool(bit_fields & (1 << 1)),
                'inverse_motor': bool(bit_fields & (1 << 2)),
                'inverse_up_limit_switch': bool(bit_fields & (1 << 3)),
                'inverse_down_limit_switch': bool(bit_fields & (1 << 4)),
                'inverse_limit_switch': bool(bit_fields & (1 << 5)),
                'rudder': bool(bit_fields & (1 << 6)),
                'bridge_channel': bridge_channel,
                'timeout': timeout,
                'max_current_warning_limit': max_current_warning,
                'max_current_error_limit': max_current_error,
                'min_current_limit': min_current
            }
        except Exception as e:
            print(f"ChannelSettings decode error: {e}")
            return None

