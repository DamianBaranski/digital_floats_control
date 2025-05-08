# Digital Floats Control

A Python application for controlling digital floats via serial communication.

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
pc_app/
├── src/                      # Source code
│   ├── core/                 # Core logic and serial communication
│   ├── ui/                   # User interface components
│   │   ├── widgets/         # Custom UI widgets
│   │   └── theme.py         # UI styling
│   └── utils/               # Helper functions
├── resources/               # Images and configuration files
└── main.py                 # Application entry point
```

The project is organized into:
- `core/`: Handles the main business logic and serial communication with the floats
- `ui/`: Contains all UI components and styling
- `utils/`: Shared utility functions
- `resources/`: Static files like images and JSON configurations

## Dependencies

- Python >= 3.6
- pyserial >= 3.5
- requests >= 2.31.0
- Pillow >= 10.0.0 