
# Digital Floats Control System

The **Digital Floats Control System** is a device designed to manage and control the gear mechanisms in airplane floats, ensuring smooth transitions between landing on land or water. This document outlines the system's operational procedures, including self-test routines, test procedures, and LED indicator states.

## System Overview

The device features two switches and five RGB LEDs. The left switch controls the gear, while the right switch controls the rudder. Four LEDs display the status of the four gears, and the fifth LED shows the status of the rudder. The device can operate in three distinct modes:

1. **Land Mode (For takeoff/landing on land):** 
   - **Gear:** Extended 
   - **Rudder:** Retracted 
   - **Switch Positions:** Gear switch down, rudder switch inactive (either up or down).

2. **Water Mode (For takeoff/landing on water):**
   - **Gear:** Retracted
   - **Rudder:** Retracted
   - **Switch Positions:** Gear switch up, rudder switch up.

3. **Maneuver Mode (For maneuvering in water):**
   - **Gear:** Retracted
   - **Rudder:** Extended
   - **Switch Positions:** Gear switch up, rudder switch down.

## Power-Up and Self-Test Procedure

Upon powering up, the Digital Floats Control System automatically performs a self-test to ensure that all critical components are functioning correctly. The results of this self-test are displayed on the RGB LEDs.

- **Error Reporting:** Any issues detected during the self-test are indicated on the RGB LEDs with a specific error code.
- **Gear State Indication:** After the self-test completes, the current gear state is displayed on the RGB LEDs.

### Self-Test Procedure

The self-test includes the following steps:

1. **I2C Device Verification:** The system verifies connections to all registered I2C devices.
2. **RTC (Real-Time Clock) Check:** The system checks the accuracy of the RTC.
3. **External Memory Verification:** The system confirms the connection to external memory.

### Operational Test Procedure

The operational test procedure ensures the system functions correctly under controlled conditions. The procedure includes:

1. **I2C Device Verification:** The system verifies connections to all registered I2C devices.
2. **Initial Voltage and Current Check:** The system reads voltage and current values from the INA219 sensor, which should display 0mV and 0mA.
3. **Relay Activation:**
   - The system activates Relay A and Relay B.
   - It then reads the voltage and current values from the INA219 sensor again. The voltage should match the power supply voltage, and the current should remain at 0mA.
4. **Relay Deactivation:** The system deactivates Relay A and Relay B.

## LED Status Indicators

The system uses RGB LEDs to indicate the current status of the gear and rudder, as well as to report any warnings or errors. The LED states are as follows:

- **Green:** Gear is extended for landing on land; rudder is retracted.
- **Blue:** Gear is retracted for landing on water; rudder is extended.
- **Yellow (Blinking):** A warning has been detected. The number of blinks corresponds to the specific warning code.
- **Red (Blinking):** An error has been detected. The number of blinks corresponds to the specific error code.

## System Connections and Setup

The device supports four actuators for the gear and one actuator for the rudder. Each actuator must be equipped with two limit switches: one for the "up" position and one for the "down" position. The device supports both Normally Open (NO) and Normally Closed (NC) limit switches. These settings can be configured for each channel using the accompanying PC application.

## PC Application

A PC application is provided to configure the device, upgrade device firmware, and download logs. Before initial use, the user must configure parameters for each channel, including:

- **Maximum and Minimum Current for the Actuator:** Set the thresholds for the actuator's operating current.
- **Maximum and Minimum Time for Actuator Movement:** Define the time limits for the actuator's movement.
- **Actuator Direction:** Specify the direction in which the actuator should move.
- **Limit Switch Assignment:** Assign which limit switch corresponds to the "down" position and which to the "up" position.
- **Limit Switch Type:** Select the type of limit switch used (Normally Open (NO) or Normally Closed (NC)).


The application provides a user-friendly interface for making these configurations, ensuring that the Digital Floats Control System is properly set up for your specific needs.

# Project Structure Documentation

## README.md Version

# Project Structure

This repository contains firmware for STM32F1-based embedded system and accompanying PC application. The project is organized into several main components:

## Directory Structure

```
.
├── application/          # Main firmware application
├── bootloader/          # Device bootloader
├── common/              # Shared libraries and drivers
├── pc_app/             # PC control application
├── doc/                # Project documentation
└── tools/              # Development tools and scripts
```

### Firmware Components

#### Application and Bootloader
Both `application/` and `bootloader/` directories share similar structure:
- `app/` - Core application logic
- `bsp/` - Board Support Package for STM32F103
- `STM32F103XB_FLASH.ld` - Linker script
- `CMakeLists.txt` - Build configuration

#### Common Libraries (`common/`)
Contains shared code used by both application and bootloader:

- **Architecture Support** (`arch/stm32f103/`)
  - Hardware abstraction layers
  - CMSIS and HAL drivers
  - MCU-specific implementations

- **Device Drivers** (`drivers/`)
  - INA219 current/voltage monitor
  - PCF8574 I/O expander
  - W25X Flash memory
  - WS2812 LED controller

- **Interfaces** (`itf/`)
  - Common hardware interfaces
  - Driver interfaces
  - HAL interfaces

- **Support Utilities** (`sup/`)
  - Base64 encoding
  - Logging functionality
  - Communication protocols
  - Settings management

### PC Application (`pc_app/`)
Python-based GUI application for device control:
- Communication protocol implementation
- Firmware upload functionality
- GUI widgets for device control and monitoring

### Development Tools (`tools/`)
- OpenOCD debugging configuration
- CMake toolchain configuration
- Version control scripts

## Build System
The project uses CMake build system. Each major component contains its own `CMakeLists.txt` file.

---

## Confluence Version

[TOC]

h1. Project Structure Overview

This document describes the structure of our STM32F1 embedded system project, which consists of firmware components and a PC control application.

h2. Main Components

The project is divided into several major components:

h3. 1. Firmware Components

h4. Application and Bootloader
* Location: */application/* and */bootloader/*
* Purpose: Main firmware and bootloader implementation
* Structure:
** app/ - Core application code
** bsp/ - Board Support Package
** Build and linker configuration files

h4. Common Libraries
* Location: */common/*
* Contains:
** Hardware abstraction layers
** Device drivers
** Interface definitions
** Support utilities

{panel:title=Architecture Support (arch/stm32f103/)}
* Hardware abstraction implementations
* CMSIS library integration
* STM32 HAL drivers
* MCU-specific code
{panel}

{panel:title=Device Drivers (drivers/)}
* INA219 - Current/voltage monitoring
* PCF8574 - I/O expansion
* W25X - Flash memory
* WS2812 - LED control
{panel}

{panel:title=Interfaces (itf/)}
* Hardware interface definitions
* Driver interfaces
* HAL interfaces
{panel}

{panel:title=Support Utilities (sup/)}
* Base64 encoding/decoding
* Logging system
* Protocol implementations
* Settings management
{panel}

h3. 2. PC Application

* Location: */pc_app/*
* Technology: Python with GUI
* Features:
** Device communication
** Firmware upload capability
** Monitoring and control interface

h3. 3. Development Tools

* Location: */tools/*
* Contents:
** OpenOCD configuration
** Toolchain settings
** Version control tools

h2. Build System

The project uses CMake build system with the following characteristics:
* Hierarchical build structure
* Separate configurations for each component
* Cross-compilation support for embedded targets

h2. Development Guidelines

h3. Adding New Components
# Place hardware-specific code in appropriate arch/ subdirectory
# Implement hardware abstractions using existing interfaces
# Add new drivers to common/drivers/
# Update CMake configuration as needed

