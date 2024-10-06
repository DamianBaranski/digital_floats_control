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
        self.values['voltage'] = None
        self.values['current'] = None
        self.values['state'] = None
        self.values['up_switch'] = None
        self.values['down_switch'] = None

    def get(self, key):
        return self.values.get(key, '')

    def fromByteArray(self, data):
        # Unpack the byte array into individual fields
        unpacked_data = struct.unpack('<hhBB', data)
        voltage = unpacked_data[0] * 0.001  # Voltage scaled by 0.1
        current = unpacked_data[1] * 0.001  # Current scaled by 0.1
        state = unpacked_data[2]  # State from the byte
        switches = unpacked_data[3]  # Switch status from the byte

        # Update values in the dictionary
        self.values['voltage'] = f"{voltage:.2f}"
        self.values['current'] = f"{current:.2f}"
        self.values['state'] = self.getStateName(state)
        self.values['up_switch'] = 'ON' if switches & 0x01 else 'OFF'
        self.values['down_switch'] = 'ON' if switches & 0x02 else 'OFF'

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
            f"Voltage: {self.values['voltage']}V\n"
            f"Current: {self.values['current']}A\n"
            f"State: {self.values['state']}\n"
            f"Up Switch: {self.values['up_switch']}\n"
            f"Down Switch: {self.values['down_switch']}\n"
        )

    def getVoltage(self):
        if not self.values['voltage']:
            return 'N/A'
        return f"{self.values['voltage']} V"

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