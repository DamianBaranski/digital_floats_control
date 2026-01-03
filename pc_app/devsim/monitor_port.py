#!/usr/bin/env python3
"""
Monitor data written to a pseudo-terminal device.
Shows both hex and ASCII representation with timestamps.
"""
import sys
import time
import select
import os

def monitor_port(port_path):
    """Monitor a pseudo-terminal port and display all data."""
    if not os.path.exists(port_path):
        print(f"Error: Port {port_path} does not exist")
        return
    
    print(f"Monitoring {port_path}")
    print("Press Ctrl+C to stop")
    print("=" * 60)
    print()
    
    try:
        with open(port_path, 'rb') as port:
            buffer = b''
            
            while True:
                # Use select for non-blocking read
                readable, _, _ = select.select([port], [], [], 0.1)
                
                if readable:
                    chunk = port.read(1024)
                    if chunk:
                        buffer += chunk
                        
                        # Process complete lines (terminated by \r or \n)
                        while b'\r' in buffer or b'\n' in buffer:
                            if b'\r' in buffer:
                                idx = buffer.index(b'\r')
                                line = buffer[:idx]
                                buffer = buffer[idx+1:]
                            elif b'\n' in buffer:
                                idx = buffer.index(b'\n')
                                line = buffer[:idx]
                                buffer = buffer[idx+1:]
                            else:
                                break
                            
                            display_data(line)
                else:
                    # Flush any remaining data
                    if buffer:
                        display_data(buffer)
                        buffer = b''
                        
    except KeyboardInterrupt:
        print("\n\nMonitoring stopped")
    except Exception as e:
        print(f"\nError: {e}")


def display_data(data):
    """Display data in both hex and ASCII format."""
    timestamp = time.strftime("%H:%M:%S.%f")[:-3]
    
    # Hex representation
    hex_str = ' '.join(f'{b:02X}' for b in data)
    
    # ASCII representation (replace non-printable with '.')
    ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data)
    
    # Try to decode as base64 (protocol uses base64)
    try:
        import base64
        decoded = base64.b64decode(data.strip())
        decoded_hex = ' '.join(f'{b:02X}' for b in decoded)
        decoded_info = f" | Decoded: {decoded_hex}"
    except:
        decoded_info = ""
    
    print(f"[{timestamp}]")
    print(f"  Hex:   {hex_str}")
    print(f"  ASCII: {ascii_str}{decoded_info}")
    print()


if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else "/dev/pts/3"
    monitor_port(port)

