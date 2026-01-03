#!/usr/bin/env python3
"""
Test script to verify simulator is working.
This simulates what the PC app does - sends a request and expects a response.
"""
import serial
import time
import sys
import base64
from protocol import Protocol

def test_simulator(port_path):
    """Test the simulator by sending a StatusData request."""
    print(f"Connecting to {port_path}...")
    
    try:
        ser = serial.Serial(port_path, baudrate=115200, timeout=2.0)
        print(f"Connected! Waiting for data...")
        
        # Send StatusData request (command 'S')
        # Format matches PC app: cmd (1) + len (1) + crc (2) + data, then base64 + \r
        frame = bytes([ord('S'), 0]) + (0).to_bytes(2, 'big') + b''
        request = base64.b64encode(frame) + b'\r'
        print(f"Sending request: {request.hex()}")
        print(f"Request (base64): {request}")
        ser.write(request)
        ser.flush()
        
        # Wait for response
        print("Waiting for response...")
        time.sleep(0.5)
        
        if ser.in_waiting:
            response = ser.readline()
            print(f"Received {len(response)} bytes")
            print(f"Response (hex): {response.hex()}")
            print(f"Response (raw): {response}")
            
            # Try to decode
            decoded = Protocol.decode_request(response)
            if decoded:
                cmd, payload = decoded
                print(f"Decoded - Cmd: {cmd}, Payload length: {len(payload)}")
                print(f"Payload (hex): {payload.hex()}")
            else:
                print("Failed to decode response")
        else:
            print("No response received!")
            
        ser.close()
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else "/dev/pts/3"
    test_simulator(port)

