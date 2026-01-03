#!/bin/bash
# Monitor data written to a pseudo-terminal device
# Usage: ./monitor_port.sh /dev/pts/3

PORT="${1:-/dev/pts/3}"

if [ ! -e "$PORT" ]; then
    echo "Error: Port $PORT does not exist"
    exit 1
fi

echo "Monitoring $PORT (Ctrl+C to stop)"
echo "=================================="
echo ""

# Method 1: Hex dump with timestamps
# hexdump -C "$PORT" | while IFS= read -r line; do
#     echo "[$(date +%H:%M:%S.%3N)] $line"
# done

# Method 2: Read and display as hex + ASCII
cat "$PORT" | hexdump -C | while IFS= read -r line; do
    echo "[$(date +%H:%M:%S.%3N)] $line"
done

