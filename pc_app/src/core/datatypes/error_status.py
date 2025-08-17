from enum import Enum
import struct


class ChannelError(Enum):
    RELAY_COMMUNICATION_ERROR = 0
    ENDSTOP_SHORT_CIRCUIT = 1
    OVER_CURRENT_ERROR = 2


class ChannelWarning(Enum):
    MOVEMENT_TIMEOUT = 0
    OVER_CURRENT_WARNING = 1
    UNDER_CURRENT_WARNING = 2
    ADC_COMMUNICATION_ERROR = 3


class SystemWarning(Enum):
    LOW_VOLTAGE = 0
    HIGH_VOLTAGE = 1
    EXT_MEMORY_ERROR = 2


class ErrorStatus:
    def __init__(self, num_channels=6):
        self.num_channels = num_channels
        self.setDefaults()

        # struct format: 6 errors + 6 warnings + 1 system warning
        self._fmt = f"<{self.num_channels*2 + 1}B"
        self._size = struct.calcsize(self._fmt)

    def setDefaults(self):
        self.values = {
            "errors": [set() for _ in range(self.num_channels)],
            "warnings": [set() for _ in range(self.num_channels)],
            "system_warnings": set(),
        }

    # ---------- Getters ----------
    def isSet(self, channel, item):
        if isinstance(item, ChannelError):
            return (
                channel < self.num_channels and item in self.values["errors"][channel]
            )
        elif isinstance(item, ChannelWarning):
            return (
                channel < self.num_channels and item in self.values["warnings"][channel]
            )
        elif isinstance(item, SystemWarning):
            return item in self.values["system_warnings"]
        return False

    def anyError(self, channel):
        return channel < self.num_channels and len(self.values["errors"][channel]) > 0

    def anyWarning(self, channel):
        return (
            channel < self.num_channels and len(self.values["warnings"][channel]) > 0
        )

    # ---------- Setters ----------
    def set(self, channel_or_item, item=None):
        if isinstance(channel_or_item, int) and isinstance(item, ChannelError):
            if channel_or_item < self.num_channels:
                self.values["errors"][channel_or_item].add(item)

        elif isinstance(channel_or_item, int) and isinstance(item, ChannelWarning):
            if channel_or_item < self.num_channels:
                self.values["warnings"][channel_or_item].add(item)

        elif isinstance(channel_or_item, SystemWarning):
            self.values["system_warnings"].add(channel_or_item)

    # ---------- Clear ----------
    def clear(self, channel_or_item, item=None):
        if isinstance(channel_or_item, int) and isinstance(item, ChannelError):
            if channel_or_item < self.num_channels:
                self.values["errors"][channel_or_item].discard(item)

        elif isinstance(channel_or_item, int) and isinstance(item, ChannelWarning):
            if channel_or_item < self.num_channels:
                self.values["warnings"][channel_or_item].discard(item)

        elif isinstance(channel_or_item, SystemWarning):
            self.values["system_warnings"].discard(channel_or_item)

    def clearAll(self, channel):
        if channel < self.num_channels:
            self.values["errors"][channel].clear()
            self.values["warnings"][channel].clear()

    def clearAllSystemWarnings(self):
        self.values["system_warnings"].clear()

    # ---------- Serialization ----------
    def toByteArray(self) -> bytes:
        err_bytes = []
        warn_bytes = []

        # Errors
        for ch in range(self.num_channels):
            mask = 0
            for e in self.values["errors"][ch]:
                mask |= 1 << e.value
            err_bytes.append(mask & 0xFF)

        # Warnings
        for ch in range(self.num_channels):
            mask = 0
            for w in self.values["warnings"][ch]:
                mask |= 1 << w.value
            warn_bytes.append(mask & 0xFF)

        # System warnings
        sys_mask = 0
        for sw in self.values["system_warnings"]:
            sys_mask |= 1 << sw.value

        packed = struct.pack(self._fmt, *(err_bytes + warn_bytes + [sys_mask]))
        return packed

    def fromByteArray(self, data: bytes):
        try:
            if len(data) != self._size:
                raise ValueError(f"Expected {self._size} bytes, got {len(data)}")

            unpacked = struct.unpack(self._fmt, data)

            # Reset
            self.setDefaults()

            # Errors
            for ch in range(self.num_channels):
                mask = unpacked[ch]
                for e in ChannelError:
                    if mask & (1 << e.value):
                        self.values["errors"][ch].add(e)

            # Warnings
            for ch in range(self.num_channels):
                mask = unpacked[self.num_channels + ch]
                for w in ChannelWarning:
                    if mask & (1 << w.value):
                        self.values["warnings"][ch].add(w)

            # System warnings
            sys_mask = unpacked[self.num_channels * 2]
            for sw in SystemWarning:
                if sys_mask & (1 << sw.value):
                    self.values["system_warnings"].add(sw)

        except Exception as e:
            print(f"Problem with Errors struct: {e}, data: {data}")
            self.setDefaults()

    # ---------- String Representation ----------
    def __str__(self):
        out = ["Errors("]
        for ch in range(self.num_channels):
            out.append(
                f"  Channel {ch}: "
                f"Errors={list(e.name for e in self.values['errors'][ch])}, "
                f"Warnings={list(w.name for w in self.values['warnings'][ch])}"
            )
        out.append(
            f"  SystemWarnings={list(sw.name for sw in self.values['system_warnings'])}"
        )
        out.append(")")
        return "\n".join(out)
