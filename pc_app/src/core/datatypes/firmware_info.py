import struct

class FirmwareInfo:
    def __init__(self):
        # struct format: 20s * 5 + 40s = 140bytes
        # 20s for app_version, hardware_version, serial_number, build_date, build_time, git_commit
        self._fmt = '<20s20s20s20s20s40s'
        self._size = struct.calcsize(self._fmt)
        self.setDefaults()

    def setDefaults(self):
        self.values = {
            'app_version': '',
            'hardware_version': '',
            'serial_number': '',
            'build_date': '',
            'build_time': '',
            'git_commit': ''
        }

    def get(self, key):
        return self.values.get(key, '')

    def get_app_version(self):
        return self.values['app_version']
    
    def get_hardware_version(self):
        return self.values['hardware_version']
    
    def get_serial_number(self):
        return self.values['serial_number']
    
    def get_build_date(self):
        return self.values['build_date']
    
    def get_build_time(self):
        return self.values['build_time']
    
    def get_git_commit(self):
        return self.values['git_commit']

    def fromByteArray(self, data: bytes):
        try:
            if len(data) != self._size:
                raise ValueError(f"Expected {self._size} bytes, got {len(data)}")

            unpacked = struct.unpack(self._fmt, data)
            self.values['app_version'] = unpacked[0].decode('utf-8').rstrip('\x00')
            self.values['hardware_version'] = unpacked[1].decode('utf-8').rstrip('\x00')
            self.values['serial_number'] = unpacked[2].decode('utf-8').rstrip('\x00')
            self.values['build_date'] = unpacked[3].decode('utf-8').rstrip('\x00')
            self.values['build_time'] = unpacked[4].decode('utf-8').rstrip('\x00')
            self.values['git_commit'] = unpacked[5].decode('utf-8').rstrip('\x00')

        except Exception as e:
            print(f"Problem with AppVersionData struct: {e}, data length={len(data)}")
            self.setDefaults()

    def toByteArray(self) -> bytes:
        try:
            # encode & pad with nulls
            packed = struct.pack(
                self._fmt,
                self.values['app_version'].encode('utf-8')[:20].ljust(20, b'\x00'),
                self.values['hardware_version'].encode('utf-8')[:20].ljust(20, b'\x00'),
                self.values['serial_number'].encode('utf-8')[:20].ljust(20, b'\x00'),
                self.values['build_date'].encode('utf-8')[:20].ljust(20, b'\x00'),
                self.values['build_time'].encode('utf-8')[:20].ljust(20, b'\x00'),
                self.values['git_commit'].encode('utf-8')[:40].ljust(40, b'\x00'),
            )
            return packed
        except Exception as e:
            print(f"Problem packing AppVersionData: {e}")
            return b''

    def __str__(self):
        return (
            f"AppVersionData(\n"
            f"  app_version      = {self.get_app_version()},\n"
            f"  hardware_version = {self.get_hardware_version()},\n"
            f"  serial_number    = {self.get_serial_number()},\n"
            f"  build_date       = {self.get_build_date()},\n"
            f"  build_time       = {self.get_build_time()},\n"
            f"  git_commit       = {self.get_git_commit()},\n"
            f")"
        )
