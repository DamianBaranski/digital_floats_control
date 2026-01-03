#!/usr/bin/env python3
"""
Non-blocking monitor that doesn't interfere with the connection.
Uses inotify to detect when data is available, then reads it.
"""
import sys
import os
import time
import select

def monitor_port_nonblocking(port_path):
    """Monitor port without blocking it."""
    if not os.path.exists(port_path):
        print(f"Error: Port {port_path} does not exist")
        return
    
    print(f"Monitoring {port_path} (non-blocking)")
    print("This will show data as it's written, but won't interfere with connections")
    print("Press Ctrl+C to stop")
    print("=" * 60)
    print()
    
    # Open in non-blocking mode
    try:
        fd = os.open(port_path, os.O_RDONLY | os.O_NONBLOCK)
        
        buffer = b''
        last_activity = time.time()
        
        while True:
            readable, _, _ = select.select([fd], [], [], 0.5)
            
            if readable:
                try:
                    chunk = os.read(fd, 1024)
                    if chunk:
                        buffer += chunk
                        last_activity = time.time()
                        
                        # Try to process complete messages
                        while b'\r' in buffer:
                            idx = buffer.index(b'\r')
                            line = buffer[:idx]
                            buffer = buffer[idx+1:]
                            
                            display_data(line)
                except BlockingIOError:
                    pass
                except OSError as e:
                    if e.errno != 11:  # EAGAIN
                        print(f"Read error: {e}")
                        break
            else:
                # Flush buffer if no activity for a while
                if buffer and (time.time() - last_activity) > 1.0:
                    display_data(buffer)
                    buffer = b''
                    
    except KeyboardInterrupt:
        print("\n\nMonitoring stopped")
    except Exception as e:
        print(f"\nError: {e}")
        import traceback
        traceback.print_exc()
    finally:
        try:
            os.close(fd)
        except:
            pass


def display_data(data):
    """Display data in both hex and ASCII format."""
    timestamp = time.strftime("%H:%M:%S.%f")[:-3]
    
    # Hex representation
    hex_str = ' '.join(f'{b:02X}' for b in data)
    
    # ASCII representation
    ascii_str = ''.join(chr(b) if 32 <= b < 127 else '.' for b in data)
    
    print(f"[{timestamp}] {len(data)} bytes")
    print(f"  Hex:   {hex_str}")
    print(f"  ASCII: {ascii_str}")
    print()


if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else "/dev/pts/3"
    monitor_port_nonblocking(port)

