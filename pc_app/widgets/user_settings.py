import struct

class UserSettings:
    def __init__(self):
        self.values = {}
        self.setDefaults()

    def setDefaults(self):
        self.values['brightness'] = 128  # Byte value (0-255)
        self.values['ldg_up_color'] = '#0000FF'  # Hex color
        self.values['ldg_down_color'] = '#00FF00'
        self.values['rudder_up_color'] = '#00FF00'
        self.values['rudder_down_color'] = '#0000FF'
        self.values['rudder_inactive_color'] = '#FFFF00'
        self.values['warning_color'] = '#FFA500'
        self.values['error_color'] = '#FF0000'

    def get(self, key):
        return self.values.get(key, '')

    def set(self, key, value):
        self.values[key] = value

    def _color_to_int(self, color):
        """Convert hex color string to 32-bit integer."""
        color = color.lstrip('#')
        color = int(color, 16)
        return color

    def _int_to_color(self, value):
        """Convert 32-bit integer to hex color string."""
        hex_color = f"{value:08X}"  # Convert to 8-char hex with leading zeroes
        hex_color = f"#{hex_color[-6:]}"      
        return hex_color  # Return in #RRGGBB format

    def toByteArray(self):
        """Pack settings into a byte array suitable for a C struct."""
        brightness = int(self.values['brightness'])
        ldg_up_color = self._color_to_int(self.values['ldg_up_color'])
        ldg_down_color = self._color_to_int(self.values['ldg_down_color'])
        rudder_up_color = self._color_to_int(self.values['rudder_up_color'])
        rudder_down_color = self._color_to_int(self.values['rudder_down_color'])
        rudder_inactive_color = self._color_to_int(self.values['rudder_inactive_color'])
        warning_color = self._color_to_int(self.values['warning_color'])
        error_color = self._color_to_int(self.values['error_color'])

        # Pack the data into a byte array using struct
        packed_data = struct.pack(
            '<IIIIIIIB',  # B = unsigned char (brightness), I = unsigned int (colors)
            ldg_up_color,
            ldg_down_color,
            rudder_up_color,
            rudder_down_color,
            rudder_inactive_color,
            warning_color,
            error_color,
            brightness
        )
        return packed_data

    def fromByteArray(self, data):
        """Unpack the byte array into the settings."""
        unpacked_data = struct.unpack('<IIIIIIIB', data[0:29])
        self.values['ldg_up_color'] = self._int_to_color(unpacked_data[0])
        self.values['ldg_down_color'] = self._int_to_color(unpacked_data[1])
        self.values['rudder_up_color'] = self._int_to_color(unpacked_data[2])
        self.values['rudder_down_color'] = self._int_to_color(unpacked_data[3])
        self.values['rudder_inactive_color'] = self._int_to_color(unpacked_data[4])
        self.values['warning_color'] = self._int_to_color(unpacked_data[5])
        self.values['error_color'] = self._int_to_color(unpacked_data[6])
        self.values['brightness'] = unpacked_data[7]

