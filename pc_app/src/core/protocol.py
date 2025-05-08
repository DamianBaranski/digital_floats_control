import base64
import binascii
import logging
from typing import Generic, TypeVar, Union, Optional
from ..ui.widgets.panels.user_settings_panel import UserSettingsPanel
from ..ui.widgets.panels.channel_settings import ChannelSettings
from ..ui.widgets.panels.monitoring_data import MonitoringData

# Configure logging with INFO level and log format
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")
logger = logging.getLogger(__name__)

# Define generic types for protocol input and output data
DataInType = TypeVar('DataInType')
DataOutType = TypeVar('DataOutType')

class Protocol(Generic[DataInType, DataOutType]):
    """
    A class for handling encoding and decoding of protocol data.
    """
    class InData:
        """Encapsulates command input data with optional CRC."""
        def __init__(self, cmd: str, data: Union[DataInType] = bytes(), crc: int = 0):
            self.cmd = cmd  # Command character
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1  # Data length
            self.data = data  # Data payload
            self.crc = crc  # Cyclic Redundancy Check for error checking

    class OutData:
        """Encapsulates decoded command output data with optional CRC."""
        def __init__(self, cmd: str, data: Union[DataOutType], crc: int = 0):
            self.cmd = cmd  # Command character
            self.len = len(data) if isinstance(data, (bytes, bytearray)) else 1  # Data length
            self.data = data  # Data payload
            self.crc = crc  # CRC

    def encode_output(self, in_data: 'Protocol.InData') -> bytes:
        """
        Encodes the command and data into a Base64 frame with length and CRC.
        """
        try:
            # Convert data to bytes and build frame with command, length, data, and CRC
            data_bytes = bytes(in_data.data)
            frame = bytes([in_data.len]) + data_bytes + in_data.crc.to_bytes(2, 'big')
            frame = base64.b64encode(bytes([ord(in_data.cmd)]) + frame) + b'\r'
            logger.debug(f"Encoded frame: {frame}")
            return frame
        except Exception as e:
            logger.error(f"Encoding error: {e}")
            return b""

    def decode_response(self, instr: str) -> Optional[Union[DataOutType]]:
        """
        Decodes a Base64 encoded response and extracts command, data, and CRC.
        """
        try:
            logger.debug(f"Received response: {instr}")
            decoded_bytes = base64.b64decode(instr)
            cmd = chr(decoded_bytes[0])  # Command character
            data_len = decoded_bytes[1]  # Length of the data
            data = decoded_bytes[2:-2]  # Exclude cmd, len, and CRC
            crc = int.from_bytes(decoded_bytes[-2:], 'big')  # Extract CRC

            out_data = self.OutData(cmd, data, crc)
            logger.debug(f"Decoded data: cmd={cmd}, data_len={data_len}, crc={crc}")
            return out_data.data
        except Exception as e:
            logger.error(f"Decoding error: {e}")
            return None

    def calculate_crc(self, data: bytes) -> int:
        """
        Calculates the CRC for the given data.
        """
        # Placeholder for CRC calculation, which should be implemented as needed
        return 0

def version_callback(response, protocol, fnc):
    """
    Callback for handling version response data.
    Decodes the response and calls fnc with version information.
    """
    if len(response) == 0:
        fnc("N/A")
    else:
        fnc(protocol.decode_response(response).decode('utf-8').rstrip('\x00'))

class AppProtocol:
    """
    Application protocol interface that wraps UART communications for
    various device functions such as version, settings, and firmware upload.
    """
    def __init__(self, comport):
        self.protocol = Protocol[Union[bytes, int], Union[bytes, int]]()  # Initialize Protocol
        self.uart = comport  # UART communication interface

    def getLogs(self):
        return self.uart.getLogs()
    
    def getVersion(self, fnc):
        """
        Sends a command to retrieve the version. Calls fnc with the result.
        """
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc("N/A")
            return
        
        try:
            # Prepare version command, encode, and send it
            cmd_str = self.protocol.InData(cmd='v')
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send_receive(encoded_cmd, lambda response: version_callback(response, self.protocol, fnc))
            logger.debug("Sent getVersion command.")
    
        except Exception as e:
            logger.error(f"Error in getVersion: {e}")
            fnc('N/A')
            
    def scanI2c(self, address, fnc):
        """
        Sends a command to scan a specific I2C address.
        Calls fnc with True if the scan is successful, False otherwise.
        """
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc(False)
            return

        try:
            # Encode and send scan command for the specified I2C address
            cmd_str = self.protocol.InData(cmd='s', data=address.to_bytes())
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send_receive(encoded_cmd, lambda response: fnc(bool().from_bytes(self.protocol.decode_response(response))))
            logger.info(f"Sent scanI2C command for address {address}.")
    
        except Exception as e:
            logger.error(f"Error in scanI2C: {e}")
            fnc(False)

    def getUserSettings(self, fnc):
        """
        Retrieves user settings from the device.
        Calls fnc with the UserSettings object.
        """
        settings = UserSettingsPanel()
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc(settings)
            return

        try:
            def callback(response):
                if len(response) > 0:
                    settings.fromByteArray(self.protocol.decode_response(response))
                fnc(settings)
                    
            # Prepare and send command to get user settings
            cmd_str = self.protocol.InData(cmd='u')
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send_receive(encoded_cmd, callback)
            logger.info("Sent getUserSettings command.")

        except Exception as e:
            logger.error(f"Error in getUserSettings: {e}")
            fnc(settings)

    def updateUserSettings(self, settings, fnc=None):
        """
        Updates the device with the given user settings.
        Calls fnc with the result of the update.
        """
        if fnc is None:
            fnc = lambda x: x  # Default function if none provided
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc(False)
            return
        try:
            # Encode and send update command with user settings data
            cmd_str = self.protocol.InData(cmd='U', data=settings.toByteArray())
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send_receive(encoded_cmd, lambda response: fnc(self.protocol.decode_response(response)))
            logger.info("Sent updateUserSettings command.")

        except Exception as e:
            logger.error(f"Error in updateUserSettings: {e}")
            fnc(False)

    def getChannelSettings(self, channel, fnc):
        settings = ChannelSettings()
        if not self.uart.isOpen():
            fnc(settings)
            return
        
        try:
            cmd_str = self.protocol.InData(cmd='c', data=channel.to_bytes(1))
            encoded_cmd = self.protocol.encode_output(cmd_str)
            
            self.uart.send_receive(encoded_cmd, lambda response: (
                settings.fromByteArray(self.protocol.decode_response(response)) if len(response) != 0 
                else settings,
                fnc(settings)
            ))

        except:
            fnc(settings)
            
    def updateChannelSettings(self, channel, settings, fnc=None):
        if fnc == None:
            fnc = lambda x: x
        if not self.uart.isOpen():
            fnc(False)
            return
        try:
            cmd_str = self.protocol.InData(cmd='C', data=settings.toByteArray()+channel.to_bytes(1))
            encoded_cmd = self.protocol.encode_output(cmd_str)

            self.uart.send_receive(encoded_cmd, lambda response: (
                fnc(self.protocol.decode_response(response))
            ))

        except:
            fnc(False)
        
    def getMonitoringData(self, channel, fnc):
        monitoringData = MonitoringData()
        if not self.uart.isOpen():
            fnc(monitoringData)
        
        try:
            cmd_str = self.protocol.InData(cmd='m', data=channel.to_bytes(1))
            encoded_cmd = self.protocol.encode_output(cmd_str)
        
            # Send and receive response, call fnc depending on whether response is None
            self.uart.send_receive(encoded_cmd, lambda response: (
                monitoringData.fromByteArray(self.protocol.decode_response(response)) if len(response) != 0 
                else monitoringData,
                fnc(monitoringData)
            ))

        except:
            fnc(monitoringData)
            
    def testRelays(self, pcf_addr, pcf_channel, ina_addr, fnc):
        print("Protocol: test relays, pcf_addr:", pcf_addr, " pcf_channel:", pcf_channel, " ina_addr:", ina_addr)
        if not self.uart.isOpen():
            fnc(False)
        
        try:
            cmd_str = self.protocol.InData(cmd='t', data=ina_addr.to_bytes(1)+pcf_addr.to_bytes(1)+pcf_channel.to_bytes(1))
            encoded_cmd = self.protocol.encode_output(cmd_str)
        
            # Send and receive response, call fnc depending on whether response is None
            self.uart.send_receive(encoded_cmd, lambda response: (
                fnc(bool().from_bytes(self.protocol.decode_response(response)))
            ))

        except:
            fnc(False)
            
    def reset(self, fnc):
        """
        Sends a command to reset the device.
        Calls fnc with True if the reset command is successful.
        """
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc(False)
            return
        
        try:
            # Prepare and send reset command
            cmd_str = self.protocol.InData(cmd='r')
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send(encoded_cmd)
            logger.debug("Sent reset command.")
            fnc(True)
        except Exception as e:
            logger.error(f"Error in reset: {e}")
            fnc(False)

    def uploadFirmware(self, fnc, data, ptr, length):
        """
        Uploads firmware to the device in chunks.
        Calls fnc with True if the upload is successful, False otherwise.
        """
        if not self.uart.isOpen():
            logger.warning("UART port is not open.")
            fnc(False)
            return
        
        try:
            logger.info(f"Uploading firmware data at ptr: {ptr} with length: {length}")
            # Encode the firmware data with pointer and length
            cmd_str = self.protocol.InData(cmd='u', data=ptr.to_bytes(2, 'little') + length.to_bytes(2, 'little') + bytes(data))
            encoded_cmd = self.protocol.encode_output(cmd_str)
            self.uart.send_receive(encoded_cmd, lambda response: fnc(bool().from_bytes(self.protocol.decode_response(response))))
            logger.debug(f"Sent firmware data: {binascii.hexlify(bytes(data))}")

        except Exception as e:
            logger.error(f"Error in uploadFirmware: {e}")
            fnc(False)
