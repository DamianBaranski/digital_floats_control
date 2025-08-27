
import struct

class ChannelSettings:
    def __init__(self):
        self.values = {}
        self.setDefaults()

    def setDefaults(self):
        self.values['channel'] = 0  # Channel number
        self.values['enable'] = False
        self.values['bridge'] = False #if true, only bridge channel is used, others filds are ignored
        self.values['inverse_motor'] = False
        self.values['inverse_up_limit_switch'] = False
        self.values['inverse_down_limit_switch'] = False
        self.values['inverse_limit_switch'] = False
        self.values['rudder'] = False
        self.values['timeout'] = 0  # Movement timeout in seconds
        self.values['bridge_channel'] = 0  # Channel number for bridge
        self.values['max_current_warning_limit'] = 0
        self.values['max_current_error_limit'] = 0
        self.values['min_current_limit'] = 0

    def get(self, key):
        return self.values.get(key, '')

    def set(self, key, value):
        self.values[key] = value

    def toByteArray(self):
        # Convert bit fields into a single byte
        bit_fields = (
            (self.values['enable'] << 0) |
            (self.values['bridge'] << 1) |
            (self.values['inverse_motor'] << 2) |
            (self.values['inverse_up_limit_switch'] << 3) |
            (self.values['inverse_down_limit_switch'] << 4) |
            (self.values['inverse_limit_switch'] << 5) |
            (self.values['rudder'] << 6)
        )

        # Pack all values into a byte array using struct.pack
        # '<' means little-endian, 'B' is for uint8_t and 'H' is for uint16_t
        packed_data = struct.pack(
            '<BBBBHHH',
            self.values['channel'],          # 1st byte for channel number 
            bit_fields,                         # 2nd byte for bitfields
            self.values['bridge_channel'],     # 3rd byte for bridge channel
            self.values['timeout'],              #4th byte for timeout
            self.values['max_current_warning_limit'] * 1000,   # 5th and 6th byte
            self.values['max_current_error_limit'] * 1000,     # 7th and 8th byte
            self.values['min_current_limit']  * 1000           # 9th and 10th byte
        )

        return packed_data

    def fromByteArray(self, data):
        # Unpack the byte array into individual fields
        unpacked_data = struct.unpack('<BBBBHHH', data)

        # Extract the bitfields from the first byte
        self.values['channel'] = unpacked_data[0]  # Channel number
        bit_fields = unpacked_data[1]
        self.values['enable'] = bool(bit_fields & (1 << 0))
        self.values['bridge'] = bool(bit_fields & (1 << 1))
        self.values['inverse_motor'] = bool(bit_fields & (1 << 2))
        self.values['inverse_up_limit_switch'] = bool(bit_fields & (1 << 3))
        self.values['inverse_down_limit_switch'] = bool(bit_fields & (1 << 4))
        self.values['inverse_limit_switch'] = bool(bit_fields & (1 << 5))
        self.values['rudder'] = bool(bit_fields & (1 << 6))
        self.values['bridge_channel'] = unpacked_data[2]  # Bridge channel
        self.values['timeout'] = unpacked_data[3]  # Movement timeout in seconds
        self.values['max_current_warning_limit'] = unpacked_data[4] * 0.001  # Convert back to A units
        self.values['max_current_error_limit'] = unpacked_data[5] * 0.001  # Convert back to A units
        self.values['min_current_limit'] = unpacked_data[6] * 0.001  # Convert back to A units
        
            # Adding the __str__ method for human-readable output
    def __str__(self):
        return (
            f"ChannelSettings:\n"
            f"  channel: {self.values['channel']}\n"
            f"  enable: {self.values['enable']}\n"
            f"  bridge: {self.values['bridge']}\n"
            f"  inverse_motor: {self.values['inverse_motor']}\n"
            f"  inverse_up_limit_switch: {self.values['inverse_up_limit_switch']}\n"
            f"  inverse_down_limit_switch: {self.values['inverse_down_limit_switch']}\n"
            f"  inverse_limit_switch: {self.values['inverse_limit_switch']}\n"
            f"  rudder: {self.values['rudder']}\n"
            f"  bridge_channel: {self.values['bridge_channel']}\n"
            f"  timeout: {self.values['timeout']} seconds\n"
            f"  max_current_warning_limit: {self.values['max_current_warning_limit']} (A units)\n"
            f"  max_current_error_limit: {self.values['max_current_error_limit']} (A units)\n"
            f"  min_current_limit: {self.values['min_current_limit']} (A units)\n"
        )