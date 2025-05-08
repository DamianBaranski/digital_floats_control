# Digital Floats Control

A Python application for controlling digital floats via serial communication.

## Installation

1. Clone the repository
2. Install the package:
```bash
pip install -e .
```

## Running the Application

Run the main application:
```bash
python main.py
```

## Project Structure

```
pc_app/
├── src/                      # Main source code directory
│   ├── core/                 # Core application logic
│   ├── ui/                   # UI components
│   └── utils/               # Utility functions
├── resources/              # Static resources
├── requirements.txt        # Project dependencies
└── main.py                # Entry point
```

## Dependencies

- Python >= 3.6
- pyserial >= 3.5
- requests >= 2.31.0
- Pillow >= 10.0.0 