
import struct

class ChannelSettings:
    def __init__(self):
        self.values = {}
        self.setDefaults()

    def setDefaults(self):
        self.values['enable'] = False
        self.values['bridge'] = False
        self.values['inverse_motor'] = False
        self.values['inverse_up_limit_switch'] = False
        self.values['inverse_down_limit_switch'] = False
        self.values['inverse_limit_switch'] = False
        self.values['rudder'] = False
        self.values['ina_addr'] = 0
        self.values['ina_calibration'] = 0
        self.values['pcf_addr'] = 0
        self.values['pcf_channel'] = 0
        self.values['max_voltage_limit'] = 0
        self.values['min_voltage_limit'] = 0
        self.values['max_current_limit'] = 0
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
            '<B B H B B H H H H', 
            bit_fields,                         # 1st byte for bitfields
            self.values['ina_addr'],            # 2nd byte
            self.values['ina_calibration'],     # 3rd and 4th byte
            self.values['pcf_addr'],            # 5th byte
            self.values['pcf_channel'],         # 6th byte
            self.values['max_voltage_limit'],   # 7th and 8th byte
            self.values['min_voltage_limit'],   # 9th and 10th byte
            self.values['max_current_limit'],   # 11th and 12th byte
            self.values['min_current_limit']    # 13th and 14th byte
        )

        return packed_data

    def fromByteArray(self, data):
        # Unpack the byte array into individual fields
        unpacked_data = struct.unpack('<BBHBBHHHH', data)

        # Extract the bitfields from the first byte
        bit_fields = unpacked_data[0]
        self.values['enable'] = bool(bit_fields & (1 << 0))
        self.values['bridge'] = bool(bit_fields & (1 << 1))
        self.values['inverse_motor'] = bool(bit_fields & (1 << 2))
        self.values['inverse_up_limit_switch'] = bool(bit_fields & (1 << 3))
        self.values['inverse_down_limit_switch'] = bool(bit_fields & (1 << 4))
        self.values['inverse_limit_switch'] = bool(bit_fields & (1 << 5))
        self.values['rudder'] = bool(bit_fields & (1 << 6))

        # Set the remaining fields
        self.values['ina_addr'] = unpacked_data[1]
        self.values['ina_calibration'] = unpacked_data[2]
        self.values['pcf_addr'] = unpacked_data[3]
        self.values['pcf_channel'] = unpacked_data[4]
        self.values['max_voltage_limit'] = unpacked_data[5]
        self.values['min_voltage_limit'] = unpacked_data[6]
        self.values['max_current_limit'] = unpacked_data[7]
        self.values['min_current_limit'] = unpacked_data[8]
        
            # Adding the __str__ method for human-readable output
    def __str__(self):
        return (
            f"ChannelSettings:\n"
            f"  enable: {self.values['enable']}\n"
            f"  bridge: {self.values['bridge']}\n"
            f"  inverse_motor: {self.values['inverse_motor']}\n"
            f"  inverse_up_limit_switch: {self.values['inverse_up_limit_switch']}\n"
            f"  inverse_down_limit_switch: {self.values['inverse_down_limit_switch']}\n"
            f"  inverse_limit_switch: {self.values['inverse_limit_switch']}\n"
            f"  rudder: {self.values['rudder']}\n"
            f"  ina_addr: 0x{self.values['ina_addr']:02X}\n"
            f"  ina_calibration: {self.values['ina_calibration']}\n"
            f"  pcf_addr: 0x{self.values['pcf_addr']:02X}\n"
            f"  pcf_channel: {self.values['pcf_channel']}\n"
            f"  max_voltage_limit: {self.values['max_voltage_limit']} (0.1V units)\n"
            f"  min_voltage_limit: {self.values['min_voltage_limit']} (0.1V units)\n"
            f"  max_current_limit: {self.values['max_current_limit']} (0.1A units)\n"
            f"  min_current_limit: {self.values['min_current_limit']} (0.1A units)\n"
        )