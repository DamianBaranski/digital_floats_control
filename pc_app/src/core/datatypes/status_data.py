import struct

class StatusData:
    def __init__(self):
        self.setDefaults()

    def setDefaults(self):
        self.values = {
            'ldg_gear_switch': None,
            'rudder_switch': None,
            'test_button': None,
            'remote_control_status': None,
            'power_voltage': None,
            'memory_usage': None,
            'uptime': None,
        }

    def get(self, key):
        return self.values.get(key, '')

    def fromByteArray(self, data):
        try:
            # Expected structure: voltage (h), memory (h), uptime (I), switches (B)
            unpacked = struct.unpack('<hhIB', data)
            self.values['power_voltage'] = unpacked[0] * 0.1
            self.values['memory_usage'] = unpacked[1]
            self.values['uptime'] = unpacked[2]

            switches = unpacked[3]
            self.values['ldg_gear_switch'] = bool(switches & 0x01)
            self.values['rudder_switch'] = bool(switches & 0x02)
            self.values['test_button'] = bool(switches & 0x04)
            self.values['remote_control_status'] = bool(switches & 0x08)

        except Exception as e:
            print(f'Problem with status struct: {e}, data: {data}')
            self.setDefaults()

    # Individual getters
    def get_ldg_gear_switch(self):
        return self.values['ldg_gear_switch']

    def get_rudder_switch(self):
        return self.values['rudder_switch']
    
    def get_remote_control_status(self):
        return self.values['remote_control_status']

    def get_test_button(self):
        return self.values['test_button']

    def get_power_voltage(self):
        return self.values['power_voltage']

    def get_memory_usage(self):
        return self.values['memory_usage']

    def get_uptime(self):
        return self.values['uptime']

    def __str__(self):
        return (
            f"StatusData(\n"
            f"  ldg_gear_switch        = {self.get_ldg_gear_switch()},\n"
            f"  rudder_switch          = {self.get_rudder_switch()},\n"
            f"  test_button            = {self.get_test_button()},\n"
            f"  remote_control_status  = {self.get_remote_control_status()},\n"
            f"  power_voltage          = {self.get_power_voltage()} V,\n"
            f"  memory_usage           = {self.get_memory_usage()} KB,\n"
            f"  uptime                 = {self.get_uptime()} s,\n"
            f")"
        )
