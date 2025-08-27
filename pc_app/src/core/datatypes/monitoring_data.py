import struct

# Simulating the State enum from C++
class State:
    UP = 0
    DOWN = 1
    MOVING = 2
    ERROR = 3

class MonitoringData:
    def __init__(self):
        self.values = {}
        self.setDefaults()

    def setDefaults(self):
        self.values['timestamp'] = None      
        self.values['current'] = None
        self.values['channel'] = None
        self.values['state'] = None
        self.values['up_switch'] = None
        self.values['down_switch'] = None

    def get(self, key):
        return self.values.get(key, '')

    def fromByteArray(self, data):
        try:
            # Unpack the byte array into individual fields
            unpacked_data = struct.unpack('<IhBBB', data)
            timestamp = unpacked_data[0]  # Timestamp in milliseconds
            current = unpacked_data[1] * 0.001  # Convert current from mA to A
            channel = unpacked_data[2]  # Channel number from the byte
            state = unpacked_data[3]  # State from the byte
            switches = unpacked_data[4]  # Switch status from the byte

            # Update values in the dictionary
            self.values['timestamp'] = timestamp
            self.values['current'] = f"{current:.2f}"
            self.values['channel'] = channel
            self.values['state'] = self.getStateName(state)
            self.values['up_switch'] = 'ON' if switches & 0x01 else 'OFF'
            self.values['down_switch'] = 'ON' if switches & 0x02 else 'OFF'
        except:
            pass
        
    def getStateName(self, state):
        if state == State.UP:
            return 'UP'
        elif state == State.DOWN:
            return 'DOWN'
        elif state == State.MOVING:
            return 'MOVING'
        elif state == State.ERROR:
            return 'ERROR'
        else:
            return 'UNKNOWN'

    def __str__(self):
        return (
            f"Monitoring Data:\n"
            f"Channel: {self.values['channel']}\n"
            f"Timestamp: {self.values['timestamp']} ms\n"
            f"Current: {self.values['current']}A\n"
            f"State: {self.values['state']}\n"
            f"Up Switch: {self.values['up_switch']}\n"
            f"Down Switch: {self.values['down_switch']}\n"
        )

    # Method to get the formatted current value
    def getCurrent(self):
        if not self.values['current']:
            return 'N/A'
        return f"{self.values['current']} A"

    # Method to get the state as a string
    def getState(self):
        if not self.values['state']:
            return 'N/A'
        return self.values['state']

    # Method to get the up switch status
    def getUpSwitch(self):
        if not self.values['up_switch']:
            return 'N/A'
        return self.values['up_switch']

    # Method to get the down switch status
    def getDownSwitch(self):
        if not self.values['down_switch']:
            return 'N/A'
        return self.values['down_switch']
    
    # Method to get the channel number
    def getChannel(self):
        if self.values['channel'] is None:
            return 'N/A'
        return str(self.values['channel'])