import struct

class MonitoringData:
    def __init__(self):
        self.values = {}
        self.setDefaults()

    def setDefaults(self):
        values['voltage'] = 'N/A'
        values['current'] = 'N/A'
        values['up_switch'] = 'N/A'
        values['down_switch'] = 'N/A'
        
    def get(self, key):
        return self.values.get(key, '')

    def fromByteArray(self, data):
        # Unpack the byte array into individual fields
        unpacked_data = struct.unpack('<HHBB', data)
        voltage = unpacked_data[0]*0.1
        current = unpacked_data[1]*0.1
        state = unpacked_data[2]
        switches = unpacked_data[3]
        
        # Adding the __str__ method for human-readable output
    def __str__(self):
        return (
            f"Monitoring data:\n"
        )