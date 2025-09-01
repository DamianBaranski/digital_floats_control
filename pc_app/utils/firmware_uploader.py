import sys
import time
import logging
from core.protocol.device_client import DeviceClient
from core.protocol import requests
from tqdm import tqdm   # <-- add this

def main():
    """Main function to handle firmware upload process."""
    if len(sys.argv) < 3:
        print("Usage: python script.py <COM_PORT> <FIRMWARE_FILE>")
        sys.exit(1)

    port_name, file_name = sys.argv[1], sys.argv[2]
    device = DeviceClient()
    device.connect(port_name)

    # Send a reset command and wait for device to reset
    device.command(requests.ResetDeviceRequest(), lambda _: None)

    firmware_info = {}

    def firmware_info_callback(info):
        firmware_info['value'] = info

    device.command(requests.FirmwareInfoRequest(), firmware_info_callback)

    timeout = 3  # seconds
    start_time = time.time()
    while 'value' not in firmware_info and time.time() - start_time < timeout:
        time.sleep(0.05)

    if 'value' not in firmware_info:
        print("Failed to retrieve firmware info.")
        sys.exit(1)

    print(f"Current Firmware Version: {firmware_info['value']}")

    # Check bootloader mode
    app_version = firmware_info['value'].get_app_version()
    if app_version != "BootBS v1.0":
        print("Device is not in bootloader mode. Please reset the device to enter bootloader mode.")
        sys.exit(1)

    # Read firmware file
    try:
        with open(file_name, 'rb') as file:
            data = file.read()
    except Exception as e:
        print(f"Error reading firmware file: {e}")
        sys.exit(1)

    chunk_size = 128
    total_chunks = (len(data) + chunk_size - 1) // chunk_size

    # Use tqdm progress bar
    with tqdm(total=total_chunks, desc="Uploading", unit="chunk") as pbar:
        for chunk in range(total_chunks):
            start = chunk * chunk_size
            end = min(start + chunk_size, len(data))
            chunk_data = data[start:end]
            upload_result = {}

            def upload_callback(res):
                upload_result['done'] = res

            device.command(requests.UploadFirmwareRequest(chunk_data, start), upload_callback)

            timeout = 5  # seconds
            start_time = time.time()
            while not upload_result.get('done'):
                if time.time() - start_time > timeout:
                    print(f"Timeout while uploading chunk {chunk + 1}")
                    sys.exit(1)
                time.sleep(0.05)

            pbar.update(1)  # update progress bar

if __name__ == "__main__":
    main()
