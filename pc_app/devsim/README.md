# Device Simulator

A TkInter application that simulates the Digital Floats device using a virtual COM port (PTY).

## Features

- **Virtual COM Port**: Creates a pseudo-terminal that appears as a real serial port
- **Tab Layout**: Separate tab for each data type (StatusData, FirmwareInfo, etc.)
- **Real-time Protocol**: Responds to requests using the same protocol as the real device
- **Visual Controls**: Sliders, checkboxes, and presets for easy testing
- **Communication Log**: Shows all RX/TX traffic

## Usage

1. Run the simulator:
   ```bash
   cd /home/xerxes-linux/digital_floats_control/pc_app/devsim
   python simulator.py
   ```

2. The simulator will display the virtual port path (e.g., `/dev/pts/3`)

3. Connect your PC app to this virtual port

4. Adjust values in the simulator tabs to test different scenarios

## StatusData Tab

Simulates the following fields:
- **Power Voltage**: 0-30V with slider control
- **Memory Usage**: 0-65535 KB
- **Uptime**: Auto-incrementing millisecond counter
- **Switches**:
  - Landing Gear Switch
  - Rudder Switch
  - Test Button
  - Remote Control Status

### Presets
- **Normal Operation**: Default values
- **Low Voltage**: Sets voltage to 9.5V
- **All Switches ON/OFF**: Bulk toggle

## Protocol

The simulator uses the same base64-encoded protocol as the real device:
- Frame format: `cmd (1 byte) + len (1 byte) + crc (2 bytes) + data`
- Terminated with `\r`
- Supports all command types: S (Status), v (Firmware), m (Monitoring), etc.

## Requirements

- Python 3.8+
- tkinter (usually included with Python)
- pyserial (optional, for reference)
- Linux (uses PTY for virtual port)

