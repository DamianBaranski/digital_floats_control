# Digital Floats Control

A manufacturing tool for digital floats device

## Quick Start

```bash
# Create and activate virtual environment
python -m venv venv
source venv/bin/activate  # On Linux/Mac
# or
.\venv\Scripts\activate  # On Windows

# Install and run
pip install -e .
python main.py
```

## Project Structure

```
pc_app/                                                # PC application
├── src/                                               # Source code
│   ├── core/                                         # Core logic and serial communication
│   │   ├── app.py                                    # Main application logic and state management
│   │   ├── protocol.py                               # Serial communication protocol implementation
│   │   └── uart.py                                   # UART communication handling and port management
│   ├── ui/                                           # User interface components
│   │   ├── widgets/                                  # Custom UI widgets
│   │   │   ├── base/                                # Base widget classes
│   │   │   │   ├── base_panel.py                    # Base panel implementation
│   │   │   │   └── base_widget.py                   # Base widget implementation
│   │   │   ├── logs/                                # Logging interface
│   │   │   │   ├── log_panel.py                     # Log display panel
│   │   │   │   └── log_handler.py                   # Log handling and formatting
│   │   │   ├── monitoring/                          # Device monitoring
│   │   │   │   ├── monitoring_panel.py              # Monitoring interface
│   │   │   │   └── channel_monitor.py               # Channel monitoring
│   │   │   ├── settings/                            # Settings interface
│   │   │   │   ├── app_settings_panel.py            # Main settings panel
│   │   │   │   ├── channel_settings.py              # Channel settings data model
│   │   │   │   ├── channel_settings_table_panel.py  # Channel settings table
│   │   │   │   ├── user_settings_panel.py           # User preferences panel
│   │   │   │   └── auto_detect.py                   # Auto-detection functionality
│   │   │   └── status/                              # Status display
│   │   │       ├── status_panel.py                  # Status information panel
│   │   │       └── connection_status.py             # Connection status display
│   │   └── theme.py                                 # UI styling and theme configuration
│   └── utils/                                        # Helper functions
│       ├── logger.py                                # Logging configuration
│       └── config.py                                # Application configuration
├── main.py                                          # Application entry point
├── setup.py                                         # Package setup configuration
└── requirements.txt                                 # Python dependencies
```

### Core Components

- `core/app.py`: Main application logic
  - Application state management
  - Command handling
  - Event processing
  - Device communication coordination

- `core/protocol.py`: Serial communication protocol
  - Protocol message encoding/decoding
  - Command definitions
  - Response handling
  - Data validation

- `core/uart.py`: UART communication
  - Serial port management
  - Connection handling
  - Data transmission
  - Port enumeration

### UI Components

- `ui/widgets/base/`:
  - `base_panel.py`: Base panel implementation
    - Common panel functionality
    - Layout management
  - `base_widget.py`: Base widget implementation
    - Common widget functionality
    - Event handling

- `ui/widgets/logs/`:
  - `log_panel.py`: Log display interface
    - Log viewing
    - Log filtering
    - Log export
  - `log_handler.py`: Log management
    - Log formatting
    - Log level handling
    - Log storage

- `ui/widgets/monitoring/`:
  - `monitoring_panel.py`: Device monitoring interface
    - Real-time monitoring
    - Data visualization
    - Status updates
  - `channel_monitor.py`: Channel monitoring
    - Channel status tracking
    - Performance monitoring
    - Error detection

- `ui/widgets/settings/`:
  - `app_settings_panel.py`: Main settings interface
    - Channel configuration
    - Device settings
    - User preferences
  - `channel_settings.py`: Channel settings data model
    - Settings structure
    - Data validation
    - Serialization
  - `channel_settings_table_panel.py`: Channel settings table
    - Settings display
    - Editing interface
    - Auto-detection integration
  - `user_settings_panel.py`: User preferences
    - Theme settings
    - Display options
    - Default values
  - `auto_detect.py`: Auto-detection
    - Device detection
    - Channel configuration
    - Settings optimization

- `ui/widgets/status/`:
  - `status_panel.py`: Status information display
    - System status
    - Device status
    - Error reporting
  - `connection_status.py`: Connection status
    - Port status
    - Connection quality
    - Error indicators

- `ui/theme.py`: UI styling
  - Color schemes
  - Font definitions
  - Layout constants
  - Style configurations

### Utilities

- `utils/logger.py`: Logging system
  - Log configuration
  - Log levels
  - Log formatting
  - Log file management

- `utils/config.py`: Application configuration
  - Default settings
  - Configuration loading
  - Settings persistence

## Dependencies

- Python >= 3.6
- pyserial >= 3.5
- requests >= 2.31.0
- Pillow >= 10.0.0 