import base64
from typing import Generic, TypeVar, Union, Optional
from widgets.user_settings import UserSettings
from widgets.channel_settings import ChannelSettings
from widgets.monitoring_data import MonitoringData

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

    def getVersion(self, fnc):
        if not self.uart.isOpen():
            fnc("N/A")
            return

        try:
            cmd_str = self.protocol.InData(cmd='v')
            encoded_cmd = self.protocol.encode_output(cmd_str)
        
            # Send and receive response, call fnc depending on whether response is None
            self.uart.send_receive(encoded_cmd, 
                                   lambda response: fnc('N/A') if len(response) == 0 
                                   else fnc(self.protocol.decode_response(response).decode('utf-8').rstrip('\x00')))
    
        except:
            fnc('N/A')

    def getLogs(self):
        return self.uart.getLogs()
    
    def scanI2c(self, address, fnc):
        if not self.uart.isOpen():
            fnc(False)
            return

        try:
            cmd_str = self.protocol.InData(cmd='s', data=address.to_bytes())
            encoded_cmd = self.protocol.encode_output(cmd_str)
        
            # Send and receive response, call fnc depending on whether response is None
            self.uart.send_receive(encoded_cmd, 
                                   lambda response: fnc(False) if len(response) == 0 
                                   else fnc(bool().from_bytes(self.protocol.decode_response(response))))
    
        except:
            fnc(False)
        
    def getUserSettings(self, fnc):
        settings = UserSettings()
        if not self.uart.isOpen():
            fnc(settings)
            return

        try:
            cmd_str = self.protocol.InData(cmd='u')
            encoded_cmd = self.protocol.encode_output(cmd_str)

            self.uart.send_receive(encoded_cmd, lambda response: (
                settings.fromByteArray(self.protocol.decode_response(response)) if len(response) != 0 
                else settings,
                fnc(settings)
            ))

        except:
            fnc(settings)
        
    def updateUserSettings(self, settings, fnc=None):
        if fnc == None:
            fnc = lambda x: x
        if not self.uart.isOpen():
            fnc(False)
            return
        try:
            cmd_str = self.protocol.InData(cmd='U', data=settings.toByteArray())
            encoded_cmd = self.protocol.encode_output(cmd_str)

            self.uart.send_receive(encoded_cmd, lambda response: (
                self.protocol.decode_response(response)
            ))

        except:
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
        