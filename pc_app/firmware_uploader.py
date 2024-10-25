import protocol
import com_port
import time
import sys
import logging

# Global variables to capture callback results
result = False
returned = None

# Configure logging
logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")
logger = logging.getLogger(__name__)

def callback(ret):
    """Callback function to capture response from protocol actions."""
    global result, returned
    result = True
    returned = ret
    logger.debug(f"Callback called with return value: {ret}")

def wait(timeout=5):
    """
    Waits for a protocol response with a specified timeout.
    Polls every 1ms. Returns False if no response within the timeout.
    """
    global result
    start_time = time.time()
    while not result:
        if time.time() - start_time > timeout:
            logger.error("Timeout while waiting for response.")
            return False
        time.sleep(0.001)
    return True

def check_bootloader_version(p, interval=0.1, timeout=3):
    """
    Continuously sends the getVersion command every 100ms.
    Returns True if the bootloader version starts with "BootBS" within the timeout.
    """
    global result, returned
    start_time = time.time()
    while time.time() - start_time < timeout:
        result = False
        p.getVersion(callback)  # Send getVersion command
        if not wait():
            logger.error("Timeout while checking bootloader version.")
            return False

        # Check if version starts with "BootBS"
        if result and returned.startswith("BootBS"):
            logger.info(f"Bootloader version: {returned}")
            return True

        time.sleep(interval)

    logger.warning("Bootloader version does not start with 'BootBS' or no response received.")
    return False

def main():
    """Main function to handle firmware upload process."""
    if len(sys.argv) < 3:
        print("Usage: python script.py <COM_PORT> <FIRMWARE_FILE>")
        sys.exit(1)

    # Retrieve port and file name from arguments
    port_name = sys.argv[1]
    file_name = sys.argv[2]

    # Open communication port
    port = com_port.ComPort()
    port.open(port_name, timeout=0.2)
    time.sleep(1)  # Short delay to establish connection
    logger.info(f"Connected to port {port_name}")

    # Initialize protocol on the open port
    p = protocol.AppProtocol(port)

    # Send a reset command
    p.reset(callback)
    if not wait():
        logger.error("Failed to reset device.")
        port.stop()
        return

    # Check if bootloader version is valid
    if not check_bootloader_version(p):
        logger.warning("Prompting for manual reset due to invalid bootloader version.")
        if not check_bootloader_version(p, timeout=60):
            logger.error("Exiting due to invalid bootloader version.")
            port.stop()
            return

    logger.info(f"Firmware file: {file_name}")

    # Open firmware file for reading in binary mode
    with open(file_name, 'rb') as file:
        data = file.read()

    # Define chunk size for firmware upload
    chunk_size = 128

    # Upload firmware in chunks
    for chunk in range(0, (len(data) + chunk_size - 1) // chunk_size):
        global result
        result = False
        logger.debug(f"Writing chunk: {chunk + 1}")

        # Calculate the length of the current chunk
        start = chunk * chunk_size
        end = min(start + chunk_size, len(data))
        chunk_data = data[start:end]

        # Upload the current chunk and wait for response
        p.uploadFirmware(fnc=callback, data=chunk_data, ptr=start, length=len(chunk_data))
        if not wait():
            logger.error("Error: Timeout while uploading chunk.")
            port.stop()
            return  # Exit on upload failure

    logger.info("Firmware upload completed successfully.")
    port.stop()

if __name__ == "__main__":
    main()
