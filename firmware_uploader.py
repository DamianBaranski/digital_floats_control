import base64
import serial
import time
import struct
import sys

class ProtocolUploader:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(port, baudrate)
    
    def encode_base64(self, data):
        return base64.b64encode(data).decode('ascii')
    
    def send_command(self, command):
        print("cmd:", command)
        #self.ser.write((command + '\r').encode('ascii'))
        for d in (command + '\r'):
            self.ser.write(d.encode('ascii'))
            time.sleep(0.0001)
    
    def send_data(self, data):
        for d in data.encode('ascii'):
            self.ser.write(d)
            time.sleep(0.2)
        self.ser.write(b'\r')
    
    def upload_file(self, file_path):
        with open(file_path, 'rb') as f:
            file_data = f.read()

        # Send the 'L' command with file size
        file_size = len(file_data)
        command_data = struct.pack('<cI', b'L', file_size)
        encoded_command = self.encode_base64(command_data)
        self.send_command(encoded_command)
        self.wait_for_response()

        # Send the 'C' commands with file data in chunks
        chunk_size = 64
        for i in range(0, file_size, chunk_size):
            chunk = file_data[i:i+chunk_size]
            command_data = struct.pack('<cB', b'C', len(chunk)) + chunk
            encoded_command = self.encode_base64(command_data)
            self.send_command(encoded_command)
            self.wait_for_response()
    
    def wait_for_response(self):
        while True:
            response = self.ser.readline().decode('ascii').strip()
            print(f"Response: {response}")
            if 'Response' in response:
                return response
            else:
                self.send_data('')
    
    def wait_for_bootloader(self):
        while True:
            line = self.ser.readline().decode('ascii').strip()
            print(f"Received: {line}")
            if "Bootloader BS" in line:
                print("Bootloader BS detected, ready to upload.")
                break
    
    def close(self):
        self.ser.close()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python upload.py <path_to_your_file>")
        sys.exit(1)

    file_path = sys.argv[1]
    
    uploader = ProtocolUploader('/dev/ttyUSB0')  # Update with your actual serial port
    uploader.wait_for_bootloader()
    uploader.upload_file(file_path)
    uploader.close()
