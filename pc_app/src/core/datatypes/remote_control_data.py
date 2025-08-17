import struct

class RemoteControlData:
    def __init__(self, ldg_gear_switch_state=0, rudder_switch_state=0, test_button_state=0):
        self.ldg_gear_switch_state = ldg_gear_switch_state
        self.rudder_switch_state = rudder_switch_state
        self.test_button_state = test_button_state

    # -------- Serialization --------
    def fromByteArray(self, data):
        try:
            (bit_fields,) = struct.unpack('<B', data)

            self.ldg_gear_switch_state = (bit_fields >> 0) & 0x01
            self.rudder_switch_state   = (bit_fields >> 1) & 0x01
            self.test_button_state     = (bit_fields >> 2) & 0x01

        except Exception as e:
            print(f'Problem with remote control struct: {e}, data: {data}')
            self.ldg_gear_switch_state = 0
            self.rudder_switch_state = 0
            self.test_button_state = 0

    def toByteArray(self):
        try:
            bit_fields = (
                (self.ldg_gear_switch_state << 0) |
                (self.rudder_switch_state   << 1) |
                (self.test_button_state     << 2)
            )
            return struct.pack('<B', bit_fields)
        except Exception as e:
            print(f'Problem packing remote control data: {e}')
            return b''

    # -------- Getters --------
    def get_ldg_gear_switch_state(self):
        return self.ldg_gear_switch_state

    def get_rudder_switch_state(self):
        return self.rudder_switch_state

    def get_test_button_state(self):
        return self.test_button_state

    # -------- Setters --------
    def set_ldg_gear_switch_state(self, value: int):
        self.ldg_gear_switch_state = 1 if value else 0

    def set_rudder_switch_state(self, value: int):
        self.rudder_switch_state = 1 if value else 0

    def set_test_button_state(self, value: int):
        self.test_button_state = 1 if value else 0

    # -------- Debug string --------
    def __str__(self):
        return (
            f"RemoteControlData(\n"
            f"  ldg_gear_switch_state = {self.ldg_gear_switch_state},\n"
            f"  rudder_switch_state   = {self.rudder_switch_state},\n"
            f"  test_button_state     = {self.test_button_state},\n"
            f")"
        )
